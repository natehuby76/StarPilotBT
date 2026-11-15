"""Local, session-authenticated H.264 adapter for the existing Dom renderer."""
from contextlib import ExitStack
import logging
import select
import socket
import struct
import tempfile
import threading
import time
from pathlib import Path
from urllib.parse import urlsplit

PATH = '/api/companion/carplay-live'
CONTENT_TYPE = 'application/x-starpilot-h264'
HEADER = struct.Struct('!4sIQQII')
MAX_FRAME_BYTES = 2 * 1024 * 1024
MAX_FRAME_AGE = 0.25
STARTUP_TIMEOUT = 45.0
SESSION_SECONDS = 300.0
_slot = threading.BoundedSemaphore(1)
_log = logging.getLogger(__name__)


def packet(payload: bytes, frame_id: int, captured_ns: int, encode_ms: float, age_ms: float) -> bytes:
  if len(payload) > MAX_FRAME_BYTES:
    raise ValueError('Video frame exceeds stream limit')
  return HEADER.pack(b'SPV1', len(payload), frame_id, captured_ns,
                     min(0xffffffff, max(0, round(encode_ms * 1000))),
                     min(0xffffffff, max(0, round(age_ms * 1000)))) + payload


def peer_connected(connection) -> bool:
  readable, _, _ = select.select([connection], [], [], 0)
  return not readable or bool(connection.recv(1, socket.MSG_PEEK | socket.MSG_DONTWAIT))


def frames(authorized, connected):
  from openpilot.starpilot.system.starport.frame_source import (
    FrameRequest, FORMAT_NV12, FLAG_NV12, FLAG_ASYNC_READBACK,
  )
  from openpilot.starpilot.system.starport.hw_encoder import HardwareH264Encoder
  from openpilot.starpilot.system.starport.view import ViewSource

  with ExitStack() as resources:
    folder = Path(resources.enter_context(tempfile.TemporaryDirectory(prefix='carplay-live-', dir='/dev/shm')))
    encoder = HardwareH264Encoder(640, 360)
    resources.callback(encoder.close)
    request = FrameRequest(640, 360, 0, 0, 33333, FLAG_NV12 | FLAG_ASYNC_READBACK)
    source = ViewSource(
      'car', request, lambda event, **fields: _log.info('CarPlay renderer %s %s', event, fields),
      car_path=str(folder / 'frames'), touch_path=str(folder / 'touch.sock'),
      control_path=str(folder / 'control.sock'), renderer_log=folder / 'renderer.log',
    )
    resources.callback(source.close)
    began = time.monotonic()
    last_frame = began
    heartbeat = began
    sent = 0
    needs_keyframe = True
    while time.monotonic() - began < SESSION_SECONDS:
      if not authorized() or not connected():
        return
      now = time.monotonic()
      source.check(now)
      if source.fallback_reason:
        raise RuntimeError('Dom renderer became unavailable')
      source.demand(1.0)
      frame = source.latest()
      if frame is None:
        deadline = STARTUP_TIMEOUT if sent == 0 else 5.0
        if now - last_frame > deadline:
          raise TimeoutError('Dom renderer did not supply a fresh frame')
        if now - heartbeat >= 1:
          heartbeat = now
          yield packet(b'', 0, 0, 0, 0)
        time.sleep(0.005)
        continue
      age = time.monotonic_ns() - frame.captured_ns
      if not 0 <= age <= MAX_FRAME_AGE * 1e9:
        if now - last_frame > (STARTUP_TIMEOUT if sent == 0 else 5):
          raise TimeoutError('Dom renderer frames are stale')
        continue
      encode = encoder.encode_nv12 if frame.pixel_format == FORMAT_NV12 else encoder.encode_rgba
      data, _ = encode(frame.data, keyframe=needs_keyframe or sent % 30 == 0)
      age_ms = (time.monotonic_ns() - frame.captured_ns) / 1e6
      if not authorized() or not connected():
        return
      if age_ms > MAX_FRAME_AGE * 1000:
        needs_keyframe = True
        continue
      yield packet(data, frame.frame_id, frame.captured_ns, encoder.last_encode_ms, age_ms)
      needs_keyframe = False
      sent += 1
      last_frame = time.monotonic()
    _log.info('CarPlay stream session ended; sent=%s', sent)


def serve(handler):
  if not handler.direct_local():
    handler.json(403, {'error': 'Live View requires local Wi-Fi', 'code': 'local_only'})
    return
  if urlsplit(handler.path).query:
    handler.json(400, {'error': 'Unexpected Live View query'})
    return
  if not handler.require_session():
    return
  identity = handler.settings_session()
  if identity is None:
    return
  if not _slot.acquire(blocking=False):
    handler.json(409, {'error': 'Live View is already in use'})
    return
  original_timeout = None
  stream = None
  try:
    original_timeout = handler.connection.gettimeout()
    # Small dependent pictures must leave immediately rather than wait for a
    # delayed TCP acknowledgement. Keep the socket's outbound queue bounded.
    handler.connection.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    handler.connection.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 64 * 1024)
    handler.connection.settimeout(3)
    handler.close_connection = True
    handler.send_response(200)
    handler.send_header('Content-Type', CONTENT_TYPE)
    handler.send_header('Cache-Control', 'no-store')
    handler.send_header('Connection', 'close')
    handler.end_headers()
    stream = frames(
      lambda: handler.direct_local() and handler.settings_session() == identity,
      lambda: peer_connected(handler.connection),
    )
    for record in stream:
      handler.wfile.write(record)
      handler.wfile.flush()
  except (OSError, RuntimeError, ValueError, ImportError):
    _log.exception('CarPlay live stream stopped')
  finally:
    try:
      if stream is not None:
        stream.close()
    finally:
      try:
        handler.connection.settimeout(original_timeout)
      finally:
        _slot.release()
