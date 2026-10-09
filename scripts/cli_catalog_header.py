#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Embed declared CLI translations without linking Qt to the standalone renderer."""
import argparse
import json
from pathlib import Path
import xml.etree.ElementTree as ET
p = argparse.ArgumentParser()
p.add_argument('--data', type=Path, required=True)
p.add_argument('--output', type=Path, required=True)
a = p.parse_args()
sources = json.loads((a.data / 'cli-sources.json').read_text(encoding='utf-8'))
assert len(sources) == len(set(sources)) and all(isinstance(s, str) and s for s in sources)
rows = []
languages = json.loads((a.data / 'catalogs.json').read_text(encoding='utf-8'))
for language in languages:
    tag = language['tag']
    messages = {m.findtext('source'): m for m in ET.parse(a.data / f'soundcurrent_{tag}.ts').findall('.//message')}
    for source in sources:
        m = messages[source]
        t = m.find('translation')
        if t is None or t.get('type') == 'unfinished' or not t.text:
            raise ValueError(f'Incomplete CLI translation: {tag}: {source}')
        rows.append('    {' + ', '.join(json.dumps(x, ensure_ascii=False) for x in (tag, source, t.text)) + '},')
a.output.parent.mkdir(parents=True, exist_ok=True)
a.output.write_text('''// Generated from reviewed Qt catalogs. SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <string_view>
namespace soundcurrent::cli {
struct Entry { std::string_view language, source, text; };
inline constexpr Entry entries[] = {
''' + '\n'.join(rows) + '\n};\n}\n', encoding='utf-8')
