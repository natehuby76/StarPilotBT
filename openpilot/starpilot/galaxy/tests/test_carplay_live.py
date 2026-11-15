import importlib.util
import io
from pathlib import Path
import socket
import sys
import types
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('carplay_live', Path(__file__).parents[1] / 'carplay_live.py')
live = importlib.util.module_from_spec(spec)
spec.loader.exec_module(live)


class Connection:
  def __init__(self):
    self.timeout = None

  def gettimeout(self):
    return self.timeout

  def settimeout(self, timeout):
    self.timeout = timeout


class Handler:
  path = live.PATH

  def __init__(self, local=True, authenticated=True):
    self.local = local
    self.session = ('test-cookie', 'generation') if authenticated else None
    self.responses = []
    self.headers = []
    self.connection = Connection()
    self.wfile = io.BytesIO()
    self.close_connection = False

  def direct_local(self):
    return self.local

  def require_session(self):
    if self.session is None:
      self.json(401, {'error': 'Refresh the local Galaxy session'})
    return self.session is not None

  def settings_session(self):
    return self.session

  def json(self, code, value):
    self.responses.append((code, value))

  def send_response(self, code):
    self.responses.append((code, None))

  def send_header(self, name, value):
    self.headers.append((name, value))

  def end_headers(self):
    pass


class CarPlayLiveTests(unittest.TestCase):
  def assert_slot_released(self):
    self.assertTrue(live._slot.acquire(blocking=False))
    live._slot.release()

  def test_binary_packet_metadata_and_size(self):
    wire = live.packet(b'video', 42, 1000, 1.25, 13.5)
    self.assertEqual(live.HEADER.unpack(wire[:live.HEADER.size]), (b'SPV1', 5, 42, 1000, 1250, 13500))
    self.assertEqual(wire[live.HEADER.size:], b'video')
    with self.assertRaises(ValueError):
      live.packet(bytes(live.MAX_FRAME_BYTES + 1), 1, 1, 0, 0)

  def test_remote_transport_rejected_before_authentication_or_capture(self):
    handler = Handler(local=False)
    with patch.object(live, 'frames') as frames:
      live.serve(handler)
    frames.assert_not_called()
    self.assertEqual(handler.responses[0][0], 403)
    self.assert_slot_released()

  def test_missing_session_rejected_without_capture(self):
    handler = Handler(authenticated=False)
    with patch.object(live, 'frames') as frames:
      live.serve(handler)
    frames.assert_not_called()
    self.assertEqual(handler.responses[0][0], 401)

  def test_queries_rejected(self):
    handler = Handler()
    handler.path += '?unexpected=1'
    live.serve(handler)
    self.assertEqual(handler.responses[0][0], 400)

  def test_concurrent_request_rejected(self):
    live._slot.acquire()
    try:
      handler = Handler()
      live.serve(handler)
      self.assertEqual(handler.responses[0][0], 409)
    finally:
      live._slot.release()

  def test_stream_uses_session_and_closes_source(self):
    handler = Handler()
    closed = []

    def frames(authorized, connected):
      try:
        self.assertTrue(authorized())
        yield b'first frame'
        handler.session = None
        self.assertFalse(authorized())
      finally:
        closed.append(True)

    with patch.object(live, 'frames', frames):
      live.serve(handler)
    self.assertEqual(handler.wfile.getvalue(), b'first frame')
    self.assertTrue(handler.close_connection)
    self.assertEqual(handler.connection.timeout, None)
    self.assertEqual(closed, [True])
    self.assert_slot_released()

  def test_encoder_failure_releases_slot(self):
    def frames(*_):
      raise RuntimeError('Encoder unavailable')
      yield

    with patch.object(live, 'frames', frames), self.assertLogs(live._log, level='ERROR'):
      live.serve(Handler())
    self.assert_slot_released()

  def test_peer_close_detected(self):
    first, second = socket.socketpair()
    try:
      self.assertTrue(live.peer_connected(first))
      second.close()
      self.assertFalse(live.peer_connected(first))
    finally:
      first.close()
      second.close()

  def test_dropping_an_encoded_frame_forces_next_keyframe(self):
    clock = types.SimpleNamespace(now=10.0)
    keyframe_requests = []
    cleanup = []

    class Encoder:
      last_encode_ms = 1.0

      def __init__(self, *_):
        pass

      def encode_nv12(self, data, *, keyframe):
        keyframe_requests.append(keyframe)
        # The first encoded frame misses its age budget. The next frame must
        # be independently decodable, since the client never saw that picture.
        clock.now += 0.3 if len(keyframe_requests) == 1 else 0.001
        return b'H264', keyframe

      def close(self):
        cleanup.append('encoder')

    class Source:
      fallback_reason = ''

      def __init__(self, *_args, **_kwargs):
        self.index = 0

      def check(self, _):
        pass

      def demand(self, _):
        pass

      def latest(self):
        self.index += 1
        return types.SimpleNamespace(
          captured_ns=int(clock.now * 1e9), frame_id=self.index,
          pixel_format=1, data=b'pixels',
        )

      def close(self):
        cleanup.append('source')

    modules = {
      'openpilot.starpilot.system.starport.frame_source': types.SimpleNamespace(
        FrameRequest=lambda *args: args, FORMAT_NV12=1, FLAG_NV12=1, FLAG_ASYNC_READBACK=2),
      'openpilot.starpilot.system.starport.hw_encoder': types.SimpleNamespace(HardwareH264Encoder=Encoder),
      'openpilot.starpilot.system.starport.view': types.SimpleNamespace(ViewSource=Source),
    }
    with patch.dict(sys.modules, modules), patch.object(live.time, 'monotonic', lambda: clock.now), \
         patch.object(live.time, 'monotonic_ns', lambda: int(clock.now * 1e9)):
      stream = live.frames(lambda: True, lambda: True)
      try:
        record = next(stream)
      finally:
        stream.close()
    self.assertEqual(keyframe_requests, [True, True])
    self.assertEqual(live.HEADER.unpack(record[:live.HEADER.size])[2], 2)
    self.assertEqual(cleanup, ['source', 'encoder'])


if __name__ == '__main__':
  unittest.main()
