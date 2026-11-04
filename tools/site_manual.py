#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Rhizome: writes site/manuel/index.html from docs/rhizome/MODE-D-EMPLOI.md (one source for the repository and the
site). A small Markdown reader for what the manual uses: headings, paragraphs, lists (two levels), numbered lists,
tables, quotes, bold, italics, code, links and SVG diagrams (inlined). Then run tools/site_nav.py for the menu."""
from pathlib import Path
import html,re
R=Path(__file__).resolve().parents[1]
SRC=R/'docs/rhizome/MODE-D-EMPLOI.md';OUT=R/'site/manuel/index.html'

def slug(t):
 t=re.sub(r'[^\w\s-]','',t.lower().replace('’',"'").replace("'",' '));return re.sub(r'\s+','-',t.strip())
def inline(t):
 t=html.escape(t,quote=False)
 t=re.sub(r'`([^`]+)`',r'<code>\1</code>',t)
 t=re.sub(r'\*\*([^*]+)\*\*',r'<b>\1</b>',t)
 t=re.sub(r'(?<![\w*])\*([^*]+)\*(?![\w*])',r'<i>\1</i>',t)
 return re.sub(r'\[([^\]]+)\]\(([^)]+)\)',lambda m:f'<a href="{m.group(2)}">{m.group(1)}</a>',t)

def render(md):
 out,toc=[],[];lines=md.split('\n');i=0
 while i<len(lines):
  l=lines[i]
  if not l.strip():i+=1;continue
  m=re.match(r'(#{1,3}) (.*)',l)
  if m:
   n=len(m.group(1));text=m.group(2);sid=slug(text)
   if n==2:toc.append((sid,text))
   out.append(f'<h{n} id="{sid}">{inline(text)}</h{n}>');i+=1;continue
  m=re.match(r'!\[([^\]]*)\]\(([^)]+\.svg)\)$',l.strip())
  if m: # a diagram: inlined, so it takes the site's colours and theme; the alt text is its caption
   svg=(SRC.parent/m.group(2)).read_text().strip()
   out.append(f'<figure class="schema">{svg}<figcaption>{inline(m.group(1))}</figcaption></figure>');i+=1;continue
  if l.startswith('|'):
   rows=[]
   while i<len(lines) and lines[i].startswith('|'):rows.append([c.strip() for c in lines[i].strip('|').split('|')]);i+=1
   head,body=rows[0],[r for r in rows[1:] if not re.match(r'^-+$',r[0].replace(':',''))]
   out.append('<div class="table"><table><thead><tr>'+''.join(f'<th>{inline(c)}</th>' for c in head)+'</tr></thead><tbody>'+
              ''.join('<tr>'+''.join(f'<td>{inline(c)}</td>' for c in r)+'</tr>' for r in body)+'</tbody></table></div>');continue
  if l.startswith('> '):
   q=[]
   while i<len(lines) and lines[i].startswith('>'):q.append(lines[i][1:].strip());i+=1
   out.append(f'<blockquote><p>{inline(" ".join(q))}</p></blockquote>');continue
  if re.match(r'(- |\d+\. )',l):
   tag='ol' if re.match(r'\d+\. ',l) else 'ul';items=[]
   while i<len(lines) and re.match(r'(\s*- |\d+\. )',lines[i]):
    s=lines[i]
    if s.startswith('  '):items[-1][1].append(re.sub(r'^\s*- ','',s))
    else:items.append([re.sub(r'^(- |\d+\. )','',s),[]])
    i+=1
   out.append(f'<{tag}>'+''.join(f'<li>{inline(t)}'+(('<ul>'+''.join(f'<li>{inline(c)}</li>' for c in sub)+'</ul>') if sub else '')+'</li>' for t,sub in items)+f'</{tag}>');continue
  p=[]
  while i<len(lines) and lines[i].strip() and not re.match(r'(#|\||> |- |\d+\. )',lines[i]):p.append(lines[i].strip());i+=1
  out.append(f'<p>{inline(" ".join(p))}</p>')
 return '\n'.join(out),toc

body,toc=render(SRC.read_text())
nav='<nav class="toc" aria-label="Sommaire"><p class="mono">Sommaire</p><ol>'+''.join(f'<li><a href="#{s}">{inline(t)}</a></li>' for s,t in toc)+'</ol></nav>'
page=f"""<!doctype html>
<html lang="fr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Rhizome · Mode d'emploi</title>
<meta name="description" content="Mode d'emploi de Rhizome, firmware ambient pour le M-VAVE FM-1 : séquenceur orbital, SIX-OP FM et STRING MACHINE, bande, réverbe HALL, nuage de grains.">
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=IBM+Plex+Sans:ital,wght@0,400;0,500;0,600;1,400&family=IBM+Plex+Mono:wght@400;500&display=swap" rel="stylesheet">
<link rel="stylesheet" href="../style.css">
<style>
.manual{{max-width:46rem}}
.manual h1{{margin-top:.4em}} .manual h2{{margin-top:2.2em;padding-top:.6em;border-top:1px solid var(--rule)}} .manual h3{{margin-top:1.6em}}
.manual .table{{overflow-x:auto;margin:1em 0}} .manual table{{border-collapse:collapse;width:100%;font-size:.94em}}
.manual th,.manual td{{text-align:left;vertical-align:top;padding:.45em .6em;border-bottom:1px solid var(--rule)}} .manual th{{color:var(--muted);font-weight:500}}
.manual td:first-child{{white-space:nowrap}}
.manual blockquote{{margin:1.2em 0;padding:.6em 1em;border-left:3px solid var(--mark);background:color-mix(in srgb,var(--paper) 85%,var(--mark))}}
.manual code{{font-family:var(--mono);font-size:.9em}}
.manual figure.schema{{margin:1.6em 0}} .manual figure.schema svg{{display:block;width:100%;max-width:540px;height:auto}} .manual figcaption{{color:var(--muted);font-size:.9em;margin-top:.4em}}
.toc{{margin:1.5em 0 2em}} .toc p{{color:var(--muted);margin:0 0 .3em}} .toc ol{{margin:0;padding-left:1.3em}}
</style>
</head>
<body>
<div class="wrap">
<header class="top mono">
  <a class="brand" href="../">Rhizome · firmware ambient pour M-VAVE FM-1</a>
  <!-- menu: generated by tools/site_nav.py -->
  <!-- /menu -->
</header>
<main class="manual">
{body.replace('</h1>','</h1>'+nav,1)}
</main>
<footer>
  <div>Rhizome, pour When Day Chokes The Night. Ce mode d'emploi est tiré de <a href="https://github.com/Dr-John-8bits/rhizome/blob/main/docs/rhizome/MODE-D-EMPLOI.md">docs/rhizome/MODE-D-EMPLOI.md</a>.</div>
  <button class="theme" id="theme" type="button">Clair / sombre</button>
</footer>
</div>
<script src="../menu.js"></script>
</body>
</html>
"""
OUT.parent.mkdir(parents=True,exist_ok=True);OUT.write_text(page);print(OUT)
