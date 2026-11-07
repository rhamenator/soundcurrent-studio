# First native Windows runtime test — 2026-11-07

Source: 59dbb99b126654a7e53b208a387fba87574c0385.
Destination: OptiPlex 7040, Windows 11 Pro x64 build 26300.
Secure Boot off; runtime Code Integrity TESTSIGN flag present; HVCI running.
Local SYS and CAT signatures and catalog membership verified. The exact test
certificate was added to LocalMachine Root/TrustedPublisher; before/after
ownership is recorded in certificate-journal.json.

Installation succeeded: Root\SOUNDCURRENTVAD hardware ID, assigned instance
ROOT\MEDIA\0002, published package oem49.inf, service SoundCurrentVAD.
Service loaded and PnP reported status OK/problem code 0. Two render endpoints
and one capture endpoint appeared. This proves load/enumeration, NOT audio.

Silent WASAPI probe failed on BOTH render endpoints and the capture endpoint:
- IMMDevice::Activate(IAudioClient): S_OK
- Activate(IAudioEndpointVolume): 0x88890008
- IAudioClient::GetMixFormat: 0x88890008
- IsFormatSupported(shared, 48000 Hz, stereo, 16-bit PCM): 0x88890008
- Initialize(shared, same PCM): 0x88890008
AUDCLNT_E_UNSUPPORTED_FORMAT prevents stream start. No audible/native playback,
microphone transport, effects, crash restoration or stress tests passed.
See native-stream-probe.txt and the adjacent probe-native-streams.cpp source.

Windows automatically made a native endpoint the default playback device after
installation. Restored all three playback roles to Realtek, with its existing
volume unchanged. Disabled ONLY ROOT\MEDIA\0002 after the failing stream tests;
the signed package remains installed for targeted debugging/removal. Realtek
and VB-CABLE drivers were retained. FxSound and SoundCurrent startup remain off.
Test mode and certificate trust remain in place for the authorized test work.

The first installer verification incorrectly matched InstanceId against the
hardware ID instead of reading HardwareID; the installer itself succeeded.
Corrected the journal and rollback identity checks. Do not repeat DevCon install
blindly: it creates another root instance. Use the exact recorded instance.

Next development step: reproduce these precise format/endpoint activation
failures on the development VM with this frozen package. Inspect kernel pin
format intersections, topology/endpoint properties and Audio Engine logs.
Do not treat device enumeration or prior VB-CABLE fixture tests as audio success.
Do not disable HVCI or loosen production trust checks to mask this format error.
