import importlib.util
import json
from pathlib import Path
import socket
import threading
import time
import unittest
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

spec = importlib.util.spec_from_file_location('warehouse_client', Path(__file__).parents[1] / 'scripts' / 'warehouse_client.py')
client_module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(client_module)
TOKEN = 'a' * 64  # Synthetic test-only token; not device credentials.


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def handle_request(self):
        count = int(self.headers.get('Content-Length', 0))
        body = json.loads(self.rfile.read(count)) if count else None
        self.server.calls.append((self.command, self.path, body, self.headers.get('Authorization')))
        pause, drop, code, response_data = self.server.sleep, self.server.drop, self.server.code, self.server.response
        if pause:
            time.sleep(pause)
        if drop:
            self.connection.shutdown(socket.SHUT_RDWR)
            self.connection.close()
            return
        response = json.dumps(response_data).encode()
        self.send_response(code)
        if code == 302:
            self.send_header('Location', '/redirected')
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(response)))
        self.end_headers()
        try:
            self.wfile.write(response)
        except OSError:
            pass

    do_GET = do_PUT = do_POST = handle_request


class ClientTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
        cls.server.daemon_threads = True
        cls.thread = threading.Thread(target=cls.server.serve_forever, daemon=True)
        cls.thread.start()

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
        cls.server.server_close()

    def setUp(self):
        self.server.calls, self.server.response = [], {'ok': True}
        self.server.code, self.server.sleep, self.server.drop = 200, 0, False
        self.client = client_module.WarehouseClient(f'http://127.0.0.1:{self.server.server_port}', timeout=.15)

    def test_command_contract(self):
        self.client.set_box('R04-B25', color=[255, 0, 0], mode='blink')
        method, path, data, auth = self.server.calls[0]
        self.assertEqual((method, path, auth), ('PUT', '/api/v1/boxes/R04-B25/state', None))
        self.assertEqual(data['duration_ms'], 60000)
        self.assertEqual(data['color'], [255, 0, 0])

    def test_off(self):
        self.client.set_box('R01-B01', False)
        self.assertEqual(self.server.calls[0][2], {'on': False})

    def test_status_boxes_all_off(self):
        self.client.status(); self.client.boxes(); self.client.all_off()
        self.assertEqual([x[0:2] for x in self.server.calls], [('GET', '/api/v1/status'), ('GET', '/api/v1/boxes'), ('POST', '/api/v1/all-off')])

    def test_http_error(self):
        self.server.code, self.server.response = 503, {'error': 'storage unavailable'}
        with self.assertRaisesRegex(client_module.ControllerError, '503'):
            self.client.status()

    def test_unknown_box(self):
        self.server.code, self.server.response = 404, {'error': 'box ID not found'}
        with self.assertRaisesRegex(client_module.ControllerError, '404'):
            self.client.set_box('MISSING')

    def test_invalid_values_never_sent(self):
        for kwargs in ({'brightness': 256}, {'color': [1, True, 3]}, {'duration_ms': 999}, {'mode': 'bad'}, {'on': 1}):
            with self.assertRaises(ValueError):
                self.client.set_box('R01-B01', **kwargs)
        with self.assertRaises(ValueError):
            self.client.set_box('R01/../B01')
        self.assertEqual(self.server.calls, [])

    def test_timeout_no_retry(self):
        self.server.sleep = .3
        with self.assertRaises(client_module.AmbiguousResult):
            self.client.set_box('R01-B01')
        self.assertEqual(len(self.server.calls), 1)

    def test_connection_drop_no_retry(self):
        self.server.drop = True
        with self.assertRaises(client_module.AmbiguousResult):
            self.client.set_box('R01-B01')
        self.assertEqual(len(self.server.calls), 1)

    def test_get_timeout(self):
        self.server.sleep = .3
        with self.assertRaises(client_module.ControllerError) as result:
            self.client.status()
        self.assertNotIsInstance(result.exception, client_module.AmbiguousResult)

    def test_redirect_not_followed(self):
        self.server.code = 302
        with self.assertRaisesRegex(client_module.ControllerError, '302'):
            self.client.status()
        self.assertEqual(len(self.server.calls), 1)

    def test_bad_origin_without_token(self):
        for origin in ('ftp://host', 'http://user:pass@host', 'http://host/api', 'http://host?x=1', ''):
            with self.assertRaises(ValueError):
                client_module.WarehouseClient(origin, TOKEN)
        client_module.WarehouseClient('http://host')


if __name__ == '__main__':
    unittest.main()
