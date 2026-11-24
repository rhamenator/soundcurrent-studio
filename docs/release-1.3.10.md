# SoundCurrent Studio 1.3.10

- Remediated CodeQL path-injection alert in `src/studio_render.cpp` by rejecting NUL bytes and raw `..` traversal segments on CLI input/output paths and canonicalizing the existing regular input file before opening.
- Remediated CodeQL integer-multiplication overflow alerts in `tests/dsp_response.cpp` and `tests/engine_response.cpp` by widening float samples to `double` before squaring and using `std::size_t` buffer indexing.
- Updated package and installer metadata for release `1.3.10`.

## Upgrade

Use **Quit**, then install 1.3.10 over the existing application. Settings, setups, and custom profiles are retained. The public Windows package continues to use the interim VB-CABLE route; the unsigned native-driver implementation remains preserved for later production signing.
