import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
import struct

spec = importlib.util.spec_from_file_location('build_metadata', Path(__file__).parents[1] / 'scripts/build_metadata.py')
metadata = importlib.util.module_from_spec(spec)
spec.loader.exec_module(metadata)


class BuildMetadataTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        sketch = self.root / '02_Firmware/WarehouseLed'
        sketch.mkdir(parents=True)
        (sketch / 'firmware_version.h').write_text('#define WAREHOUSE_FIRMWARE_VERSION "v0.7.0"')
        self.secrets = self.root / 'private.json'
        self.secrets.write_text(json.dumps(dict(ssid='HuaweiJIN', password='synthetic-test-password')))
        self.binary = self.root / 'application.bin'

    def test_fresh_identity_and_stale_rejection(self):
        first = metadata.generate(self.root, self.secrets)
        self.binary.write_bytes(b'ESP' + first['identity'].encode() + b'\0')
        result = metadata.verify(self.root, self.binary, 'synthetic-fqbn')
        self.assertEqual(result['bytes'], self.binary.stat().st_size)
        second = metadata.generate(self.root, self.secrets)
        self.assertNotEqual(first['identity'], second['identity'])
        with self.assertRaises(ValueError):
            metadata.verify(self.root, self.binary, 'synthetic-fqbn')

    def test_missing_mismatch_and_size(self):
        first = metadata.generate(self.root, self.secrets)
        self.binary.write_bytes(b'ESP')
        with self.assertRaises(ValueError):
            metadata.verify(self.root, self.binary, '')
        self.binary.write_bytes(first['identity'].encode() + b'\0' + bytes(0x140000))
        with self.assertRaises(ValueError):
            metadata.verify(self.root, self.binary, '')
        self.binary.write_bytes(first['identity'].encode() + b'\0')
        (self.root / '02_Firmware/WarehouseLed/generated_build.h').write_text('#pragma once')
        with self.assertRaises(ValueError):
            metadata.verify(self.root, self.binary, '')

    def test_secrets_not_in_metadata(self):
        metadata.generate(self.root, self.secrets)
        self.assertNotIn('synthetic-test-password', (self.root / '_sandbox/build-intent.json').read_text())
        self.secrets.write_text('{}')
        with self.assertRaises(ValueError):
            metadata.generate(self.root, self.secrets)

    def test_partition_gate(self):
        intent = metadata.generate(self.root, self.secrets)
        self.binary.write_bytes(intent['identity'].encode() + b'\0')
        table = self.root / 'partitions.bin'
        entries = [(0, 0x10, 0x10000, 0x140000, b'app0'), (0, 0x11, 0x150000, 0x140000, b'app1'),
                   (1, 2, 0x290000, 0x20000, b'nvs')]
        table.write_bytes(b''.join(struct.pack('<HBBII16sI', 0x50AA, *entry, 0) for entry in entries))
        metadata.verify(self.root, self.binary, '', table)
        table.write_bytes(bytes(32))
        with self.assertRaises(ValueError):
            metadata.verify(self.root, self.binary, '', table)
