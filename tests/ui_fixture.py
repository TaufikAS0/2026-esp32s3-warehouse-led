"""Loopback-only UI fixture. This is NOT the ESP32 WebServer runtime."""
import json
import re
from pathlib import Path
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

TOKEN = 'a' * 64  # Synthetic fixture token only.
ROOT = Path(__file__).parents[1]
VERSION = re.search(r'#define WAREHOUSE_FIRMWARE_VERSION "([^"]+)"', (ROOT / '02_Firmware/WarehouseLed/firmware_version.h').read_text()).group(1)
HTML = (ROOT / '02_Firmware/WarehouseLed/web_ui.h').read_text(encoding='utf-8').replace(')HTML" WAREHOUSE_FIRMWARE_VERSION R"HTML(', VERSION).split('R"HTML(', 1)[1].split(')HTML"', 1)[0]
CONFIG = {'schema': 1, 'brightness_limit': 128, 'outputs': [3, 0, 0, 0],
          'boxes': [{'id': 'R01-B01', 'name': 'Rak 1 Box 1', 'output': 1, 'start_led': 1, 'led_count': 3}],
          'wifi': {'ssid': '', 'password_configured': False}}
STATES = {}
HARDWARE_TESTS = {}


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def reply(self, data, code=200):
        raw = json.dumps(data).encode()
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(raw)))
        self.end_headers()
        self.wfile.write(raw)

    def dispatch(self):
        global CONFIG
        if self.path == '/' and self.command == 'GET':
            raw = HTML.encode()
            self.send_response(200)
            self.send_header('Content-Type', 'text/html; charset=utf-8')
            self.send_header('Content-Length', str(len(raw)))
            self.end_headers(); self.wfile.write(raw); return
        body_length = int(self.headers.get('Content-Length', 0))
        body = json.loads(self.rfile.read(body_length)) if body_length else {}
        if self.path == '/api/v1/config' and self.command == 'PUT':
            seen, pixels = set(), set()
            for box in body['boxes']:
                if box['id'] in seen:
                    return self.reply({'error': 'duplicate box ID'}, 400)
                seen.add(box['id'])
                for led in range(box['start_led'], box['start_led'] + box['led_count']):
                    pixel = (box['output'], led)
                    if pixel in pixels or led > body['outputs'][box['output'] - 1]:
                        return self.reply({'error': 'invalid mapping'}, 400)
                    pixels.add(pixel)
            body.setdefault('wifi', CONFIG['wifi'])
            CONFIG = body
            CONFIG['wifi'].pop('password', None)
            CONFIG['wifi']['password_configured'] = False
            STATES.clear()
            HARDWARE_TESTS.clear()
            return self.reply(CONFIG)
        if self.path in ('/api/v1/config', '/api/v1/boxes'):
            return self.reply(CONFIG)
        if self.path == '/api/v1/all-off':
            STATES.clear(); HARDWARE_TESTS.clear(); return self.reply({'ok': True})
        if self.path.startswith('/api/v1/outputs/') and self.path.endswith('/test'):
            output = int(self.path.split('/')[-2])
            if output not in range(1, 5):
                return self.reply({'error': 'output not found'}, 404)
            if body['on'] and not CONFIG['outputs'][output - 1]:
                return self.reply({'error': 'output has no installed LEDs'}, 400)
            duration = body.get('duration_ms', 3000)
            if not 1000 <= duration <= 30000:
                return self.reply({'error': 'invalid duration'}, 400)
            for box in CONFIG['boxes']:
                if box['output'] == output:
                    STATES.pop(box['id'], None)
            state = {'output': output, 'on': body['on'], 'remaining_ms': duration if body['on'] else 0,
                     'brightness': min(body.get('brightness', 128), CONFIG['brightness_limit']),
                     'color': body.get('color', [0, 255, 0]), 'mode': body.get('mode', 'steady'),
                     'led_count': CONFIG['outputs'][output - 1]}
            HARDWARE_TESTS[output] = dict(state, deadline=time.monotonic() + duration / 1000)
            return self.reply(state)
        if self.path.startswith('/api/v1/boxes/') and self.path.endswith('/state'):
            box_id = self.path.split('/')[-2]
            if box_id not in {b['id'] for b in CONFIG['boxes']}:
                return self.reply({'error': 'box ID not found'}, 404)
            state = {'id': box_id, 'on': body['on'], 'color': body.get('color', [0, 255, 0]),
                     'brightness': min(body.get('brightness', 128), CONFIG['brightness_limit']),
                     'mode': body.get('mode', 'steady'), 'remaining_ms': body.get('duration_ms', 60000)}
            STATES[box_id] = dict(state, deadline=time.monotonic() + state['remaining_ms'] / 1000)
            output = next(box['output'] for box in CONFIG['boxes'] if box['id'] == box_id)
            HARDWARE_TESTS.pop(output, None)
            return self.reply(state)
        if self.path == '/api/v1/status':
            states = []
            for b in CONFIG['boxes']:
                s = dict(STATES.get(b['id'], {'id': b['id'], 'on': False, 'remaining_ms': 0, 'brightness': 128, 'mode': 'steady', 'color': [0, 255, 0]}))
                if 'deadline' in s:
                    s['remaining_ms'] = max(0, int((s.pop('deadline') - time.monotonic()) * 1000))
                    s['on'] = s['on'] and s['remaining_ms'] > 0
                states.append(s)
            tests = []
            for output, data in HARDWARE_TESTS.items():
                test = dict(data); test['remaining_ms'] = max(0, int((test.pop('deadline') - time.monotonic()) * 1000))
                test['on'] = test['on'] and test['remaining_ms'] > 0; tests.append(test)
            return self.reply({'version': VERSION + ' [UI fixture]', 'hardware_tests': tests, 'ip': '127.0.0.1', 'sta_connected': True,
                               'ap_active': False, 'ap_ssid': 'Warehouse-FIXTURE', 'outputs': CONFIG['outputs'],
                               'brightness_limit': CONFIG['brightness_limit'], 'ota_remaining_ms': 0,
                               'led_driver_ready': True, 'storage_error': '', 'boxes': states})
        if self.path == '/api/v1/ota':
            return self.reply({'remaining_ms': 600000 if body['enabled'] else 0})
        return self.reply({'error': 'unknown fixture endpoint'}, 404)

    do_GET = do_POST = do_PUT = dispatch


if __name__ == '__main__':
    print('UI fixture only at http://127.0.0.1:8769; token = 64 lowercase a characters', flush=True)
    HTTPServer(('127.0.0.1', 8769), Handler).serve_forever()
