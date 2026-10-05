# SPDX-License-Identifier: GPL-3.0-only
"""Synthetic multichannel WAVE fixtures; never opens an audio device."""
import math
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

renderer = str(Path(sys.argv[1]).resolve())
guid_tail = bytes.fromhex('00001000800000aa00389b71')


def chunk(name, data):
    return name + struct.pack('<I', len(data)) + data + (b'\0' if len(data) % 2 else b'')


def fixture(path, channels, samples, bits=16, floating=False, extensible=False, mask=0):
    tag = 3 if floating else 1
    fmt = struct.pack('<HHIIHH', 0xfffe if extensible else tag, channels, 48000,
                      48000 * channels * (bits // 8), channels * (bits // 8), bits)
    if extensible:
        fmt += struct.pack('<HHII', 22, bits, mask, tag) + guid_tail
    if floating:
        audio = struct.pack('<' + 'f' * len(samples), *samples)
    else:
        audio = b''.join(int(s * (1 << (bits - 1))).to_bytes(bits // 8, 'little', signed=True)
                         for s in samples)
    body = b'WAVE' + chunk(b'JUNK', b'x') + chunk(b'fmt ', fmt) + chunk(b'data', audio)
    path.write_bytes(b'RIFF' + struct.pack('<I', len(body)) + body)


def output(path):
    data = path.read_bytes()
    assert data[:4] == b'RIFF' and data[8:12] == b'WAVE'
    assert struct.unpack_from('<I', data, 4)[0] + 8 == len(data)
    fmt = audio = None
    offset = 12
    while offset < len(data):
        name, size = struct.unpack_from('<4sI', data, offset)
        payload = data[offset + 8:offset + 8 + size]
        if name == b'fmt ':
            fmt = payload
        if name == b'data':
            audio = payload
        offset += 8 + size + size % 2
    assert fmt and audio is not None and len(fmt) == 40
    tag, channels, rate, byte_rate, align, bits = struct.unpack_from('<HHIIHH', fmt)
    assert tag == 0xfffe and rate == 48000 and bits == 32
    assert byte_rate == rate * channels * 4 and align == channels * 4
    assert fmt[24:28] == struct.pack('<I', 3) and fmt[28:] == guid_tail
    return channels, struct.unpack('<' + 'f' * (len(audio) // 4), audio), struct.unpack_from('<I', fmt, 20)[0]


def run(source, destination, *options, success=True):
    result = subprocess.run([renderer, '--input', str(source), '--output', str(destination), *options],
                            capture_output=True, text=True, timeout=20)
    assert (result.returncode == 0) == success, (result.stdout, result.stderr)
    return result


with tempfile.TemporaryDirectory(prefix='soundcurrent-studio-wave-') as directory:
    root = Path(directory)
    for channels in (1, 3, 6, 8, 32):
        for bits in (16, 24, 32):
            source, dest = root / 'in.wav', root / f'roundtrip-{channels}-{bits}.wav'
            samples = [(i % 9 - 4) / 16 for i in range(channels * 1025)]
            fixture(source, channels, samples, bits, extensible=bits == 32)
            run(source, dest)
            count, decoded, mask = output(dest)
            assert count == channels and list(decoded) == samples and mask == 0

    source = root / 'in.wav'
    fixture(source, 6, [.1, .2, .3, .4, .5, .6], floating=True, bits=32, extensible=True, mask=0x3f)
    preserved = root / 'preserved.wav'
    run(source, preserved)
    assert output(preserved)[2] == 0x3f
    mixed = root / 'mixed.wav'
    run(source, mixed, '--output-channels', '2', '--route', '1:1:0', '--route', '1:3:-6.020599913',
        '--route', '2:2:0', '--route', '2:3:-6.020599913')
    channels, decoded, mask = output(mixed)
    assert channels == 2 and mask == 0 and abs(decoded[0]-.25) < 1e-6 and abs(decoded[1]-.35) < 1e-6

    fixture(source, 8, [.125 if i == 5 else 0 for i in range(8 * 1025)])
    echo = root / 'echo.wav'
    run(source, echo, '--delay-mix', '1', '--delay-ms', '10', '--delay-feedback', '.5', '--tail', '.1')
    channels, decoded, mask = output(echo)
    assert channels == 8 and len(decoded) == (1025+4800)*8
    assert decoded[480*8+5] == .125 and decoded[960*8+5] == .0625
    assert all(value == 0 for i, value in enumerate(decoded) if i % 8 != 5)

    fixture(source, 3, [math.nan, math.inf, 2], floating=True, bits=32)
    sanitized = root / 'sanitized.wav'
    result = run(source, sanitized)
    assert 'invalid samples: 2' in result.stdout and 'clipped samples: 1' in result.stdout
    assert output(sanitized)[1] == (0, 0, 1)

    protected = root / 'keep.wav'
    protected.write_bytes(b'KEEP THIS')
    run(source, protected, success=False)
    assert protected.read_bytes() == b'KEEP THIS'
    run(source, source, success=False)
    for option, value in (('--output-channels', '257'), ('--tail', 'nan'), ('--delay-feedback', '1.1'),
                          ('--gain', '4:0'), ('--eq', '1:1000:99:1'), ('--route', '1:4:0')):
        dest = root / 'rejected.wav'
        run(source, dest, option, value, success=False)
        assert not dest.exists()

    valid = source.read_bytes()
    for index, malformed in enumerate((b'', valid[:15], valid[:-1], b'RF64'+valid[4:],
                                       valid[:4]+struct.pack('<I', 0xffffffff)+valid[8:])):
        broken, dest = root / 'bad.wav', root / f'bad-out-{index}.wav'
        broken.write_bytes(malformed)
        run(broken, dest, success=False)
        assert not dest.exists()
    # Header-declared oversized chunks must fail without allocating their size.
    body = b'WAVEfmt ' + struct.pack('<I', 0xffffffff)
    broken.write_bytes(b'RIFF'+struct.pack('<I', len(body))+body)
    run(broken, root / 'oversized.wav', success=False)
    assert not list(root.glob('.soundcurrent-render-*'))
print('PASS: PCM16/24/32, 1/3/6/8/32-channel streaming, extensible metadata, explicit downmix, tails, bad files and no-overwrite output')
