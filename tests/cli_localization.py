# SPDX-License-Identifier: GPL-3.0-only
"""Exercise compiled standalone catalog selection with no audio devices."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
renderer = str(Path(sys.argv[1]).resolve())
rows = json.loads((root / 'data/localization/catalogs.json').read_text(encoding='utf-8'))
with tempfile.TemporaryDirectory() as directory:
    folder = Path(directory)
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
        assert not output.exists()
        assert not list(folder.glob('.soundcurrent-render-*'))
print('PASS: 34 standalone CLI catalogs, normalized tags, region fallback, UTF-8 diagnostics and no output on failure')
