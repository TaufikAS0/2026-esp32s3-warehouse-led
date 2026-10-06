"""JARVIS-ready stdlib client. Credentials are supplied through environment variables."""
import argparse
import http.client
import json
import os
import re
import socket
import sys
import urllib.error
import urllib.parse
import urllib.request


class ControllerError(RuntimeError):
    pass


class AmbiguousResult(ControllerError):
    """A write may have succeeded. Read status before deciding whether to repeat it."""


class WarehouseClient:
    def __init__(self, base_url, token, timeout=5):
        parsed = urllib.parse.urlsplit(base_url)
        if parsed.scheme not in ('http', 'https') or not parsed.netloc or parsed.username or parsed.password or parsed.query or parsed.fragment:
            raise ValueError('Use an HTTP(S) controller URL without credentials/query/fragment')
        if parsed.path not in ('', '/'):
            raise ValueError('URL must be controller origin, without an API path')
        if not re.fullmatch(r'[a-f0-9]{64}', token):
            raise ValueError('Token must be 64 lowercase hex characters from USB')
        if timeout <= 0:
            raise ValueError('timeout must be positive')
        self.base_url, self.token, self.timeout = base_url.rstrip('/'), token, timeout
        # Do not send LAN credentials through machine proxy settings or follow redirects.
        class NoRedirect(urllib.request.HTTPRedirectHandler):
            def redirect_request(self, req, fp, code, msg, headers, newurl):
                return None
        self.opener = urllib.request.build_opener(urllib.request.ProxyHandler({}), NoRedirect())

    def _request(self, path, method='GET', data=None):
        body = None if data is None else json.dumps(data).encode('utf-8')
        headers = {'Authorization': 'Bearer ' + self.token}
        if body is not None:
            headers['Content-Type'] = 'application/json'
        request = urllib.request.Request(self.base_url + '/api/v1' + path, data=body, headers=headers, method=method)
        try:
            with self.opener.open(request, timeout=self.timeout) as response:
                return json.load(response)
        except urllib.error.HTTPError as exc:
            try:
                message = json.loads(exc.read(32768)).get('error', 'request rejected')
            except (ValueError, AttributeError):
                message = 'request rejected'
            raise ControllerError(f'HTTP {exc.code}: {message}') from None
        except (urllib.error.URLError, TimeoutError, socket.timeout, OSError, ValueError, http.client.HTTPException) as exc:
            if method != 'GET':
                raise AmbiguousResult('Write result unknown. Read status before repeating the command; no retry was sent.') from exc
            raise ControllerError('Controller status unavailable') from exc

    def status(self):
        return self._request('/status')

    def boxes(self):
        return self._request('/boxes')

    def set_box(self, box_id, on=True, color=(0, 255, 0), brightness=128, mode='steady', duration_ms=60000):
        if not re.fullmatch(r'[A-Za-z0-9_-]{1,24}', box_id):
            raise ValueError('Invalid box ID')
        if type(on) is not bool:
            raise ValueError('on must be boolean')
        payload = {'on': on}
        if on:
            if len(color) != 3 or any(type(c) is not int or not 0 <= c <= 255 for c in color):
                raise ValueError('color must contain three integer channels 0..255')
            if type(brightness) is not int or not 0 <= brightness <= 255:
                raise ValueError('brightness must be 0..255')
            if mode not in ('steady', 'blink') or type(duration_ms) is not int or not 1000 <= duration_ms <= 3600000:
                raise ValueError('Invalid mode or duration')
            payload.update(color=list(color), brightness=brightness, mode=mode, duration_ms=duration_ms)
        return self._request('/boxes/' + box_id + '/state', 'PUT', payload)

    def all_off(self):
        return self._request('/all-off', 'POST')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['status', 'boxes', 'on', 'off', 'all-off'])
    parser.add_argument('box_id', nargs='?')
    parser.add_argument('--seconds', type=int, default=60)
    parser.add_argument('--blink', action='store_true')
    parser.add_argument('--brightness', type=int, default=128)
    parser.add_argument('--color', nargs=3, type=int, default=[0, 255, 0], metavar=('R', 'G', 'B'))
    args = parser.parse_args()
    try:
        client = WarehouseClient(os.environ.get('WAREHOUSE_LED_URL', ''), os.environ.get('WAREHOUSE_LED_TOKEN', ''))
        if args.action in ('on', 'off'):
            if not args.box_id:
                parser.error('box_id required for on/off')
            result = client.set_box(args.box_id, on=args.action == 'on', color=args.color,
                                    brightness=args.brightness, mode='blink' if args.blink else 'steady', duration_ms=args.seconds * 1000)
        elif args.action == 'all-off':
            result = client.all_off()
        else:
            result = getattr(client, args.action)()
        print(json.dumps(result, indent=2))
        return 0
    except (ValueError, ControllerError) as exc:
        print(str(exc), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
