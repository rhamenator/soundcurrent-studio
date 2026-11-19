# SoundCurrent Studio 1.3.8

- `soundcurrent-studio --help` and `--version` now print and exit instead of starting the application.
- A read-only recording mode lets the Analysis page read a chosen output's monitor in a `--preview` session. It is used for documentation footage and is off in normal use.
- `--preview` sessions are hardened: they cannot start audio processing and do not take over a running instance.

## Upgrade

Use **Quit**, then install 1.3.8 over the existing application. Settings, setups, and custom profiles are retained. The public Windows package continues to use the interim VB-CABLE route; the unsigned native-driver implementation remains preserved for later production signing.
