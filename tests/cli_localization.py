# SPDX-License-Identifier: GPL-3.0-only
"""Exercise compiled standalone catalog selection with no audio devices."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import wave
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
renderer = str(Path(sys.argv[1]).resolve())
rows = json.loads((root / 'data/localization/catalogs.json').read_text(encoding='utf-8'))
with tempfile.TemporaryDirectory() as directory:
    folder = Path(directory)
    input_wave = folder / 'valid.wav'
    with wave.open(str(input_wave), 'wb') as fixture:
        fixture.setnchannels(1)
        fixture.setsampwidth(2)
        fixture.setframerate(48000)
        fixture.writeframes(bytes(256))
    for tag, expected_tag in [(r['tag'], r['tag']) for r in rows] + [
        ('de_DE', 'de'), ('DE-de', 'de'), ('xx-Unknown', 'en'),
        ('pt_BR', 'pt-BR'), ('pt-AO', 'en'), ('zh-Unknown', 'en')]:
        messages = {m.findtext('source'): m.findtext('translation') for m in
                    ET.parse(root / f'data/localization/soundcurrent_{expected_tag}.ts').findall('.//message')}
        output = folder / 'output.wav'
        result = subprocess.run([renderer, '--language', tag, '--input', str(folder / 'missing.wav'),
                                 '--output', str(output)], capture_output=True, timeout=10)
        expected = messages['Render: %1'].replace('%1', messages['Cannot open input WAVE file'])
        assert result.returncode == 1, (tag, result.returncode)
        assert result.stderr.decode('utf-8').strip() == expected, (tag, result.stderr, expected)
        for arguments, source in [(['--post-gain'], 'Missing option value'),
                                  (['--post-gain', 'nan'], 'Invalid finite numeric argument'),
                                  (['--eq', '1:100'], 'Wrong number of colon-separated fields'),
                                  (['--output-channels', '0'], 'Channel indexes are one-based and must exist')]:
            invalid = subprocess.run([renderer, '--language', tag] + arguments,
                                     capture_output=True, timeout=10)
            expected_error = messages['Render: %1'].replace('%1', messages[source])
            assert invalid.returncode == 1
            assert invalid.stderr.decode('utf-8').strip() == expected_error, (tag, invalid.stderr)
        # Real engine configuration rejects these values after reading valid audio.
        for arguments, source in [
            (['--tail', '-1'], 'Tail must be between 0 and 30 seconds'),
            (['--tail', '31'], 'Tail must be between 0 and 30 seconds'),
            (['--post-gain', '-85'], 'Post gain must be finite and within -84 to +24 dB'),
            (['--delay-ms', '0'], 'Delay settings are outside the supported range'),
            (['--delay-feedback', '1'], 'Delay settings are outside the supported range'),
            (['--reverb-decay', '11'], 'Reverb settings are outside the supported range'),
            (['--reverb-damping', '1'], 'Reverb settings are outside the supported range'),
            (['--gain', '1:-61'], 'Invalid channel gain or too many EQ bands'),
            (['--eq', '1:0:0:1'], 'Invalid EQ band')]:
            invalid = subprocess.run([renderer, '--language', tag, '--input', str(input_wave),
                                      '--output', str(output)] + arguments,
                                     capture_output=True, timeout=10)
            expected_error = messages['Render: %1'].replace('%1', messages[source])
            assert invalid.returncode == 1
            assert invalid.stderr.decode('utf-8').strip() == expected_error, (tag, invalid.stderr)
            assert not output.exists()
        assert not output.exists()
        assert not list(folder.glob('.soundcurrent-render-*'))
    # Inclusive endpoints stay valid; the appended duration changes only frame data.
    sizes = []
    for seconds in (0, 30):
        destination = folder / f'tail-{seconds}.wav'
        result = subprocess.run([renderer, '--input', str(input_wave), '--output', str(destination),
                                 '--tail', str(seconds)], capture_output=True, timeout=10)
        assert result.returncode == 0, result.stderr
        sizes.append(destination.stat().st_size)
    assert sizes[1] - sizes[0] == 30 * 48000 * 4
print('PASS: 34 standalone CLI catalogs, normalized tags, region fallback, UTF-8 diagnostics and no output on failure')
