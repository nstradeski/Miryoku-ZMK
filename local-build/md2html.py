#!/usr/bin/env python3
"""Minimal Markdown -> standalone HTML for CUSTOM_BUILD.md.
Handles: ATX headings, GFM tables, fenced code blocks, ordered/unordered
lists, blockquotes, horizontal rules, **bold**, `inline code`. Stdlib only.
"""
import html
import re
import sys

CSS = """
:root { color-scheme: light dark; }
body { font: 16px/1.65 -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
       max-width: 860px; margin: 2.5rem auto; padding: 0 1.25rem; color: #1a1a1a; }
h1, h2, h3 { line-height: 1.25; margin-top: 2rem; }
h1 { font-size: 1.9rem; border-bottom: 2px solid #ddd; padding-bottom: .3rem; }
h2 { font-size: 1.45rem; border-bottom: 1px solid #eee; padding-bottom: .25rem; }
h3 { font-size: 1.15rem; }
code { font: .88em/1.4 ui-monospace, SFMono-Regular, Menlo, monospace;
       background: #f0f0f3; padding: .15em .4em; border-radius: 4px; }
pre { background: #1e1e22; color: #e6e6e6; padding: 1rem 1.1rem; border-radius: 8px;
      overflow-x: auto; }
pre code { background: none; padding: 0; color: inherit; font-size: .85rem; }
table { border-collapse: collapse; width: 100%; margin: 1rem 0; font-size: .92rem; }
th, td { border: 1px solid #ddd; padding: .5rem .7rem; text-align: left; vertical-align: top; }
th { background: #f5f5f7; }
tr:nth-child(even) td { background: #fafafa; }
blockquote { margin: 1rem 0; padding: .6rem 1rem; border-left: 4px solid #f0a500;
             background: #fff8e6; border-radius: 0 6px 6px 0; }
hr { border: none; border-top: 1px solid #ddd; margin: 2rem 0; }
ul, ol { padding-left: 1.4rem; }
li { margin: .25rem 0; }
@media (prefers-color-scheme: dark) {
  body { color: #e6e6e6; background: #16161a; }
  h1 { border-color: #333; } h2 { border-color: #2a2a2a; }
  code { background: #2a2a30; } th { background: #222; }
  tr:nth-child(even) td { background: #1c1c20; }
  th, td { border-color: #333; }
  blockquote { background: #2a2410; border-color: #c98a00; }
  hr { border-color: #333; }
}
"""


def inline(text):
    text = html.escape(text)
    text = re.sub(r'`([^`]+)`', r'<code>\1</code>', text)
    text = re.sub(r'\*\*([^*]+)\*\*', r'<strong>\1</strong>', text)
    return text


def cells(line):
    return [c.strip() for c in line.strip().strip('|').split('|')]


def convert(md):
    out, lines, i, n = [], md.split('\n'), 0, len(md.split('\n'))
    while i < n:
        line = lines[i]
        if line.startswith('```'):
            i += 1
            buf = []
            while i < n and not lines[i].startswith('```'):
                buf.append(html.escape(lines[i]))
                i += 1
            i += 1
            out.append('<pre><code>' + '\n'.join(buf) + '</code></pre>')
            continue
        if re.match(r'^\s*\|.*\|\s*$', line) and i + 1 < n and re.match(r'^\s*\|[-:\s|]+\|\s*$', lines[i + 1]):
            head = cells(line)
            i += 2
            rows = []
            while i < n and re.match(r'^\s*\|.*\|\s*$', lines[i]):
                rows.append(cells(lines[i]))
                i += 1
            t = ['<table><thead><tr>'] + [f'<th>{inline(h)}</th>' for h in head] + ['</tr></thead><tbody>']
            for r in rows:
                t.append('<tr>' + ''.join(f'<td>{inline(c)}</td>' for c in r) + '</tr>')
            t.append('</tbody></table>')
            out.append(''.join(t))
            continue
        m = re.match(r'^(#{1,6})\s+(.*)$', line)
        if m:
            lv = len(m.group(1))
            out.append(f'<h{lv}>{inline(m.group(2))}</h{lv}>')
            i += 1
            continue
        if re.match(r'^---+\s*$', line):
            out.append('<hr>')
            i += 1
            continue
        if line.startswith('>'):
            buf = []
            while i < n and lines[i].startswith('>'):
                buf.append(inline(lines[i].lstrip('>').strip()))
                i += 1
            out.append('<blockquote>' + '<br>'.join(buf) + '</blockquote>')
            continue
        if re.match(r'^\s*\d+\.\s+', line):
            buf = []
            while i < n and re.match(r'^\s*\d+\.\s+', lines[i]):
                buf.append('<li>' + inline(re.sub(r'^\s*\d+\.\s+', '', lines[i])) + '</li>')
                i += 1
            out.append('<ol>' + ''.join(buf) + '</ol>')
            continue
        if re.match(r'^\s*[-*]\s+', line):
            buf = []
            while i < n and re.match(r'^\s*[-*]\s+', lines[i]):
                buf.append('<li>' + inline(re.sub(r'^\s*[-*]\s+', '', lines[i])) + '</li>')
                i += 1
            out.append('<ul>' + ''.join(buf) + '</ul>')
            continue
        if line.strip() == '':
            i += 1
            continue
        buf = []
        while i < n and lines[i].strip() and not re.match(r'^(#{1,6}\s|```|>|\s*[-*]\s|\s*\d+\.\s)', lines[i]) and not re.match(r'^---+\s*$', lines[i]) and not re.match(r'^\s*\|.*\|\s*$', lines[i]):
            buf.append(inline(lines[i]))
            i += 1
        out.append('<p>' + '<br>'.join(buf) + '</p>')
    return '\n'.join(out)


src, dst = sys.argv[1], sys.argv[2]
with open(src) as f:
    body = convert(f.read())
title = "Miryoku ZMK — Custom Build"
doc = f"""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>{title}</title><style>{CSS}</style></head>
<body>{body}</body></html>"""
with open(dst, 'w') as f:
    f.write(doc)
print(f"wrote {dst}")
