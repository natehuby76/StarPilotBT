# SpacePilot

Native C4 Easter egg: in Developer settings, tap the **version number** five
times. Each tap must release within 450 ms without dragging; consecutive taps
must be 100–850 ms apart, all within three seconds. Navigation, scrolling,
touching another tile, loss of parked evidence, or a long press resets progress.

Tap the playfield to launch/boost. The wheel's + and − events are read through an
independent, non-conflated `carState` subscriber while the game is admitted.
Release a wheel button once to arm it, then each press boosts once. Held buttons,
duplicate messages and repeated press events cannot continuously boost. Exit
returns to Developer settings; an unsafe exit immediately reveals the native
road/alert screen. Best score lasts for this game session.

## Admission and lifecycle

Only the native C4 shell installs the version gesture. There is no environment
override for admission. Replay, projection, non-car devices and unknown state
are refused. One continuous second of fresh physical evidence is required:
Park, standstill, effectively zero ego/raw/all wheel speeds, valid CAN, ignition,
recognized connected Pandas, no Panda faults/control permission, no ACC, no
selfdrive or lateral/longitudinal control activity, and no active native alert.
Both source and receiver timestamps are checked with their original clocks.
The game closes on every failing check, producer gap, clock discontinuity or
wheel backlog. Safety is checked before drawing and before touch actions.

The game never writes vehicle messages, drive-mode preferences, manager state,
control permissions, process configuration or audio state. It does not start any
driving process. It uses the already-running, physically parked pipeline.

Force Offroad stops `card` and `selfdrived`, losing trustworthy gear/speed and
wheel input. The existing offroad physical-evidence fallback can confirm
ignition/noOutput but cannot provide these inputs. Therefore Force Offroad
cannot admit or sustain SpacePilot. No speculative parked process mode is added.

## Validation boundary

`test_spacepilot_state.py` covers refusal, original source/receipt clocks, dwell,
gesture timing, input edges, simulation, scoring and bounded storage.
`test_spacepilot_native.py` covers actual cereal schemas, native widget events,
return-to-road routing, lifecycle cleanup and both raylib gradient signatures.
Native GL screenshots and real isolated button IPC are host fixtures, not
physical vehicle proof. Physical C4 full-pipeline CPU/GPU/frame-time and actual
wheel validation still require an authorized, exclusively leased desk device.
Never infer road readiness or deployment from these tests.
