"""Generate per-invocation UTC identity and gate the application artifact. No upload."""
import argparse
import datetime as dt
import hashlib
import json
from pathlib import Path
import re
import struct
import uuid


def version(root):
    source = (root / '02_Firmware/WarehouseLed/firmware_version.h').read_text()
    return re.search(r'#define WAREHOUSE_FIRMWARE_VERSION "(v\d+\.\d+\.\d+)"', source)[1]


def generate(root, secrets_file):
    # Error messages deliberately omit all private field values.
    secrets = json.loads(secrets_file.read_text(encoding='utf-8-sig'))
    if secrets.get('ssid') != 'HuaweiJIN' or not isinstance(secrets.get('password'), str) or not 8 <= len(secrets['password'].encode()) <= 63:
        raise ValueError('Private lab settings require HuaweiJIN and an 8..63 byte password')
    if any(ord(c) < 32 or ord(c) == 127 for c in secrets['password']):
        raise ValueError('Private lab password contains unsupported control characters')
    utc = dt.datetime.now(dt.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')
    identity = f'WAREHOUSE_BUILD|{version(root)}|{utc}|{uuid.uuid4().hex}'
    metadata = dict(version=version(root), build_utc=utc, identity=identity)
    sketch = root / '02_Firmware/WarehouseLed'
    (sketch / 'generated_build.h').write_text(
        '#pragma once\n#define WAREHOUSE_BUILD_UTC ' + json.dumps(utc) +
        '\n#define WAREHOUSE_BUILD_IDENTITY ' + json.dumps(identity) + '\n', encoding='utf-8')
    (sketch / 'generated_lab.h').write_text(
        '#pragma once\nnamespace Lab {\nconstexpr const char* kSsid = "HuaweiJIN";\n'
        'constexpr const char* kPassword = ' + json.dumps(secrets['password']) + ';\n}\n', encoding='utf-8')
    (root / '_sandbox').mkdir(exist_ok=True)
    (root / '_sandbox/build-intent.json').write_text(json.dumps(metadata, indent=2), encoding='utf-8')
    return metadata


def verify(root, binary, fqbn, partitions=None):
    intent = json.loads((root / '_sandbox/build-intent.json').read_text())
    if intent['version'] != version(root) or not re.fullmatch(r'\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z', intent['build_utc']):
        raise ValueError('Version or UTC metadata mismatch')
    dt.datetime.strptime(intent['build_utc'], '%Y-%m-%dT%H:%M:%SZ')
    expected_prefix = f"WAREHOUSE_BUILD|{intent['version']}|{intent['build_utc']}|"
    if not re.fullmatch(re.escape(expected_prefix) + '[0-9a-f]{32}', intent['identity']):
        raise ValueError('Build identity mismatch')
    header = (root / '02_Firmware/WarehouseLed/generated_build.h').read_text()
    if json.dumps(intent['identity']) not in header or json.dumps(intent['build_utc']) not in header:
        raise ValueError('Generated header is missing or stale')
    raw = binary.read_bytes()
    if len(raw) > 0x140000 or not raw or intent['identity'].encode() + b'\0' not in raw:
        raise ValueError('Application missing current build identity or exceeds OTA slot')
    if partitions:
        records = {}
        table = partitions.read_bytes()
        for at in range(0, len(table) - 31, 32):
            magic, kind, subtype, offset, size, label, flags = struct.unpack_from('<HBBII16sI', table, at)
            if magic != 0x50AA:
                continue
            records[label.rstrip(b'\0').decode()] = (kind, subtype, offset, size)
        expected = {'app0': (0, 0x10, 0x10000, 0x140000), 'app1': (0, 0x11, 0x150000, 0x140000),
                    'nvs': (1, 2, 0x290000, 0x20000)}
        if any(records.get(k) != v for k, v in expected.items()):
            raise ValueError('Partition gate failed: expected two OTA slots and 128 KiB NVS')
    manifest = dict(version=intent['version'], build_utc=intent['build_utc'], identity=intent['identity'],
                    sha256=hashlib.sha256(raw).hexdigest(), bytes=len(raw), fqbn=fqbn, binary=binary.name,
                    hardware_verified=False, uploaded=False)
    (binary.parent / 'manifest.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['generate', 'verify'])
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--secrets', type=Path)
    parser.add_argument('--binary', type=Path)
    parser.add_argument('--partitions', type=Path)
    parser.add_argument('--fqbn', default='')
    args = parser.parse_args()
    try:
        if args.action == 'generate':
            result = generate(args.root, args.secrets or args.root / '_sandbox/lab-secrets.json')
        else:
            result = verify(args.root, args.binary, args.fqbn, args.partitions)
        print(json.dumps({k: result[k] for k in ('version', 'build_utc')}))
    except (OSError, ValueError, KeyError, TypeError):
        parser.exit(1, 'Build metadata gate failed. Check private lab file, generated metadata and application artifact.\n')


if __name__ == '__main__':
    main()
