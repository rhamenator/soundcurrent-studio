# SPDX-License-Identifier: GPL-3.0-only
"""Exercise compiled standalone catalog selection with no audio devices."""
import json
import hashlib
import re
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
    input_wave = folder / 'entrée-音声-мікрофон-🎵.wav'
    with wave.open(str(input_wave), 'wb') as fixture:
        fixture.setnchannels(1)
        fixture.setsampwidth(2)
        fixture.setframerate(48000)
        fixture.writeframes(bytes(256))
    clipping_wave = folder / 'crête-音声-🎵.wav'
    with wave.open(str(clipping_wave), 'wb') as fixture:
        fixture.setnchannels(1)
        fixture.setsampwidth(2)
        fixture.setframerate(48000)
        fixture.writeframes(b'\x00\x40' * 128)  # PCM16 amplitude 0.5.
    processing_digest = None
    for tag, expected_tag in [(r['tag'], r['tag']) for r in rows] + [
        ('de_DE', 'de'), ('DE-de', 'de'), ('xx-Unknown', 'en'),
        ('pt_BR', 'pt-BR'), ('pt-AO', 'en'), ('zh-Unknown', 'en')]:
        messages = {m.findtext('source'): m.findtext('translation') for m in
                    ET.parse(root / f'data/localization/soundcurrent_{expected_tag}.ts').findall('.//message')}
        for suffix, returncode in [(['--help'], 0), ([], 1)]:
            help_result = subprocess.run([renderer, '--language', tag] + suffix,
                                         capture_output=True, timeout=10)
            lines = help_result.stdout.decode('utf-8').splitlines()
            assert help_result.returncode == returncode
            assert lines[0] == messages['SoundCurrent Studio offline renderer (no audio device required)']
            assert lines[1] == messages['Usage: %1 [options]'].replace('%1',
                'soundcurrent-studio-render --input in.wav --output NEW.wav')
            assert lines[-1] == messages['Channel indexes start at 1. Existing output files are never overwritten.']
            assert len(lines) == 21
            assert lines[19] == messages['Input: PCM16/24/32 or float32 RIFF/WAVE. Output: float32 extensible WAVE.']
            for row, prefix, source in [
                (2, '  --language TAG         ', 'interface language; unsupported tags use English'),
                (3, '  --output-channels N    ', '1-256 output channels (default: input count)'),
                (4, '  --route OUT:IN:DB      ', 'explicit matrix gain; using any route clears defaults'),
                (5, '  --eq CH:HZ:DB:Q        ', 'peaking EQ for one output channel; repeat as needed'),
                (6, '  --lowpass CH:HZ:Q     ', 'optional channel low-pass (e.g. LFE)'),
                (7, '  --highpass CH:HZ:Q    ', 'optional channel high-pass'),
                (8, '  --gain CH:DB           ', 'output channel trim, -60 to +24 dB'),
                (9, '  --post-gain DB         ', 'overall post gain, -84 to +24 dB'),
                (10, '  --delay-ms MS          ', '1-2000 ms (default 250)'),
                (11, '  --delay-feedback F    ', '0-0.9 (default .35)'),
                (12, '  --delay-mix F         ', 'wet fraction 0-1 (enables delay)'),
                (13, '  --reverb-decay SEC    ', '.1-10 seconds (default 1.5)'),
                (14, '  --reverb-damping F    ', '0-.95 (default .4)'),
                (15, '  --reverb-mix F        ', 'wet fraction 0-1 (enables reverb)'),
                (16, '  --tail SEC            ', 'append 0-30 seconds to render effect tails'),
                (17, '  --no-headroom         ', 'disable automatic EQ headroom'),
                (18, '  --bypass              ', 'bypass EQ, effects, gains and mute')]:
                assert lines[row] == prefix + messages[source], (tag, row, lines[row])

        output = folder / 'sortie-音声-мікрофон-🎵.wav'
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
        for option in ('--unknown-option', '--unknown-%1-%2', '--unknown:option', '--équipement-音声-🎵'):
            invalid = subprocess.run([renderer, '--language', tag, option, 'unused'],
                                     capture_output=True, timeout=10)
            inner = messages['Unknown option: %1'].replace('%1', option)
            expected_error = messages['Render: %1'].replace('%1', inner)
            assert invalid.returncode == 1
            assert invalid.stderr.decode('utf-8').strip() == expected_error, (tag, invalid.stderr)
        # Real engine configuration rejects these values after reading valid audio.
        for arguments, source in [
            (['--route', '1:1:-121'], 'Route gain must be between -120 and +12 dB'),
            (['--route', '1:1:13'], 'Route gain must be between -120 and +12 dB'),
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
        clipped = subprocess.run([renderer, '--language', tag, '--input', str(clipping_wave),
                                  '--output', str(output), '--post-gain', '24', '--no-headroom'],
                                 capture_output=True, timeout=10)
        assert clipped.returncode == 0, (tag, clipped.stderr)
        summary = re.sub(r'%([1-4])', lambda m: ('1', '1', '128', '48000')[int(m[1])-1],
                         messages['Rendered %1 -> %2 channels, %3 frames at %4 Hz.'])
        statistics = re.sub(r'%([1-3])', lambda m: ('7.92447', '128', '0')[int(m[1])-1],
                            messages['Peak before clipping: %1; clipped samples: %2; invalid samples: %3'])
        assert clipped.stdout.decode('utf-8').splitlines() == [summary, statistics], (tag, clipped.stdout)
        digest = hashlib.sha256(output.read_bytes()).hexdigest()
        if processing_digest is None:
            processing_digest = digest
        assert digest == processing_digest, (tag, 'Locale changed rendered audio')
        output.unlink()
        # The inclusive boundary remains usable; it is not lowered by the alias.
        for arguments in [(['--eq', '1:100:0:1'] * 64),
                          (['--eq', '1:100:0:1'] * 33 + ['--lowpass', '1:100:1'] * 31)]:
            accepted = subprocess.run([renderer, '--language', tag, '--input', str(input_wave),
                                       '--output', str(output)] + arguments,
                                      capture_output=True, timeout=10)
            assert accepted.returncode == 0, (tag, accepted.stderr)
            summary = re.sub(r'%([1-4])', lambda m: ('1', '1', '128', '48000')[int(m[1])-1],
                             messages['Rendered %1 -> %2 channels, %3 frames at %4 Hz.'])
            statistics = re.sub(r'%([1-3])', lambda m: '0',
                                messages['Peak before clipping: %1; clipped samples: %2; invalid samples: %3'])
            assert accepted.stdout.decode('utf-8').splitlines() == [summary, statistics], (tag, accepted.stdout)

            assert output.exists()
            output.unlink()
        # Same 64-filter per-channel limit for EQ alone and mixed filter types.
        for arguments in [(['--eq', '1:100:0:1'] * 65),
                          (['--eq', '1:100:0:1'] * 33 + ['--lowpass', '1:100:1'] * 32)]:
            invalid = subprocess.run([renderer, '--language', tag, '--input', str(input_wave),
                                      '--output', str(output)] + arguments,
                                     capture_output=True, timeout=10)
            expected_error = messages['Render: %1'].replace('%1', messages['Too many Studio channel filters'])
            assert invalid.returncode == 1
            assert invalid.stderr.decode('utf-8').strip() == expected_error, (tag, invalid.stderr)
            assert not output.exists()
        assert not output.exists()
        sentinel = b'Existing user output must remain unchanged.'
        output.write_bytes(sentinel)
        existing = subprocess.run([renderer, '--language', tag, '--input', str(input_wave),
                                   '--output', str(output)], capture_output=True, timeout=10)
        expected_error = messages['Render: %1'].replace('%1', messages['Output already exists; select a new filename'])
        assert existing.returncode == 1
        assert existing.stderr.decode('utf-8').strip() == expected_error, (tag, existing.stderr)
        assert output.read_bytes() == sentinel
        output.unlink()
        assert not list(folder.glob('.soundcurrent-render-*'))
    # Inclusive endpoints stay valid; the appended duration changes only frame data.
    sizes = []
    for seconds in (0, 30):
        destination = folder / f'余韻-мікрофон-🎵-{seconds}.wav'
        result = subprocess.run([renderer, '--input', str(input_wave), '--output', str(destination),
                                 '--tail', str(seconds)], capture_output=True, timeout=10)
        assert result.returncode == 0, result.stderr
        sizes.append(destination.stat().st_size)
    assert sizes[1] - sizes[0] == 30 * 48000 * 4
print('PASS: 34 standalone CLI catalogs, normalized tags, region fallback, UTF-8 diagnostics and no output on failure')
