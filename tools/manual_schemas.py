#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""The manual's diagrams of the orbital sequencer, written to docs/rhizome/schemas/ (python3 tools/manual_schemas.py).
Computed with the firmware's own rules: gates as a euclidean rhythm, (s*k)%n<k, one per direction seen from the star,
the first at the top, turning clockwise; the time a body takes to reach each gate from Kepler's equation, M = E - e sin E.
Colours are the site's (var(--v1)...) with fallbacks, so the files also read on their own (GitHub)."""
from pathlib import Path
import math
R=Path(__file__).resolve().parents[1];OUT=R/'docs/rhizome/schemas';OUT.mkdir(parents=True,exist_ok=True)
INK='var(--ink,#7d82a8)';MUTED='var(--muted,#9aa2c4)';RULE='var(--rule,#9aa2c4)';STAR='var(--mark,#ffc668)'
V=['var(--v1,#e8735e)','var(--v2,#d9a23a)','var(--v3,#3fb98f)','var(--v4,#5b84e6)']
FONT='font-family="var(--sans,system-ui,sans-serif)"'

def euclid(k,n):return [s for s in range(n) if (s*k)%n<k]
def svg(w,h,body,title):
 return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w} {h}" role="img" aria-label="{title}" {FONT} font-size="13">'
  f'<title>{title}</title>{body}</svg>\n')
def text(x,y,s,fill=INK,anchor='middle',size=13,weight=400):
 return f'<text x="{x:.1f}" y="{y:.1f}" fill="{fill}" text-anchor="{anchor}" font-size="{size}" font-weight="{weight}">{s}</text>'
def star(x,y):return f'<circle cx="{x}" cy="{y}" r="5" fill="{STAR}"/><circle cx="{x}" cy="{y}" r="9" fill="none" stroke="{STAR}" stroke-opacity=".5"/>'
def arrow(x0,y0,x1,y1,col=MUTED):
 a=math.atan2(y1-y0,x1-x0);h=8
 p=[(x1,y1),(x1-h*math.cos(a-.4),y1-h*math.sin(a-.4)),(x1-h*math.cos(a+.4),y1-h*math.sin(a+.4))]
 return f'<line x1="{x0}" y1="{y0}" x2="{x1}" y2="{y1}" stroke="{col}" stroke-width="1.5"/><polygon points="{" ".join(f"{u:.1f},{v:.1f}" for u,v in p)}" fill="{col}"/>'

# 1. From a step sequencer to an orbit: E(5,16) as a row of steps, then wrapped round the star.
def steps_to_orbit():
 n,k=16,5;hits=euclid(k,n);b=[]
 b.append(text(20,24,'Un séquenceur pas à pas : 16 pas, 5 jouent',anchor='start',weight=600))
 for s in range(n):
  x=20+s*24;on=s in hits
  b.append(f'<rect x="{x}" y="38" width="20" height="20" rx="3" fill="{V[0] if on else "none"}" stroke="{V[0] if on else RULE}" stroke-width="1.5"/>')
  b.append(text(x+10,74,str(s+1),MUTED,size=10))
 b.append(arrow(210,92,210,122))
 b.append(text(222,112,'on enroule la boucle autour de l’étoile',MUTED,anchor='start',size=12))
 cx,cy,r=210,250,95
 b.append(f'<circle cx="{cx}" cy="{cy}" r="{r}" fill="none" stroke="{V[0]}" stroke-opacity=".55" stroke-width="1.5"/>')
 for s in range(n):
  a=-math.pi/2+2*math.pi*s/n;on=s in hits;d=9 if on else 4
  x0,y0=cx+(r-d)*math.cos(a),cy+(r-d)*math.sin(a);x1,y1=cx+(r+d)*math.cos(a),cy+(r+d)*math.sin(a)
  b.append(f'<line x1="{x0:.1f}" y1="{y0:.1f}" x2="{x1:.1f}" y2="{y1:.1f}" stroke="{V[0] if on else RULE}" stroke-width="{3 if on else 1}" stroke-linecap="round"/>')
  b.append(text(cx+(r+22)*math.cos(a),cy+(r+22)*math.sin(a)+4,str(s+1),INK if on else MUTED,size=11 if on else 9,weight=600 if on else 400))
 b.append(star(cx,cy));b.append(text(cx,cy+26,'l’étoile',MUTED,size=11))
 a=-math.pi/2+2*math.pi*2.4/n;bx,by=cx+r*math.cos(a),cy+r*math.sin(a)
 b.append(f'<circle cx="{bx:.1f}" cy="{by:.1f}" r="7" fill="{V[0]}"/>')
 tx,ty=-math.sin(a),math.cos(a) # clockwise on screen
 b.append(arrow(bx+11*tx,by+11*ty,bx+30*tx,by+30*ty,V[0]))
 b.append(text(342,206,'le corps tourne :',INK,anchor='start',size=12));b.append(text(342,222,'il joue en passant',INK,anchor='start',size=12));b.append(text(342,238,'chaque porte',INK,anchor='start',size=12))
 b.append(text(342,282,'PERIOD = la durée',MUTED,anchor='start',size=12));b.append(text(342,298,'d’un tour (16 pas)',MUTED,anchor='start',size=12))
 b.append(text(342,322,'GATES = 5 portes',MUTED,anchor='start',size=12));b.append(text(342,338,'aux pas 1, 5, 8, 11, 14',MUTED,anchor='start',size=12))
 return svg(540,400,''.join(b),'Du séquenceur pas à pas à l’orbite : 16 pas dont 5 jouent, enroulés en cercle autour de l’étoile ; les 5 pas joués deviennent 5 portes que le corps franchit')

# 2. Round, then oval: the same five gates, the same turn; on the ellipse the notes crowd near the star.
def kepler_times(e,w,k=5,n=16):
 def M_of(phi):
  nu=phi-w;E=2*math.atan2(math.sqrt(1-e)*math.sin(nu/2),math.sqrt(1+e)*math.cos(nu/2));return E-e*math.sin(E)
 start=M_of(-math.pi/2);out=[]
 for s in euclid(k,n):out.append(((M_of(-math.pi/2+2*math.pi*s/n)-start)%(2*math.pi))/(2*math.pi))
 return out
def ellipse_path(cx,cy,a,e,w):
 bb=a*math.sqrt(1-e*e);pts=[]
 for j in range(97):
  t=2*math.pi*j/96;ex,ey=a*(math.cos(t)-e),bb*math.sin(t)
  pts.append((cx+ex*math.cos(w)-ey*math.sin(w),cy+ex*math.sin(w)+ey*math.cos(w)))
 return 'M'+' L'.join(f'{x:.1f},{y:.1f}' for x,y in pts)
def round_oval():
 b=[];w=math.pi/2 # the periapsis below the star
 for col,(e,title) in enumerate([(0,'Orbite ronde : ECCENTRICITY 0'),(.6,'Orbite ovale : ECCENTRICITY 0.60')]):
  ox=20+col*250;cx,cy=ox+110,175;a=80 if e==0 else 70
  b.append(text(ox,24,title,anchor='start',weight=600))
  rx=cx-a*e*math.cos(w);ry=cy-a*e*math.sin(w) # the ellipse's centre; the star is at the focus (cx, cy)
  b.append(f'<path d="{ellipse_path(cx,cy,a,e,w)}" fill="none" stroke="{V[3]}" stroke-opacity=".6" stroke-width="1.5"/>')
  for s in euclid(5,16):
   phi=-math.pi/2+2*math.pi*s/16;nu=phi-w;r=a*(1-e*e)/(1+e*math.cos(nu))
   b.append(f'<line x1="{cx}" y1="{cy}" x2="{cx+(r+14)*math.cos(phi):.1f}" y2="{cy+(r+14)*math.sin(phi):.1f}" stroke="{RULE}" stroke-dasharray="2 3"/>')
   b.append(f'<circle cx="{cx+r*math.cos(phi):.1f}" cy="{cy+r*math.sin(phi):.1f}" r="4" fill="{V[3]}"/>')
  b.append(star(cx,cy))
  if e:b.append(text(cx,cy+a*(1-e)+24,'près de l’étoile : vite',MUTED,size=11));b.append(text(cx,cy-a*(1+e)-8,'loin : lentement',MUTED,size=11))
  # one turn as a line of time, the five notes on it
  y=310;b.append(f'<line x1="{ox}" y1="{y}" x2="{ox+220}" y2="{y}" stroke="{RULE}" stroke-width="1.5"/>')
  for q in range(5):b.append(f'<line x1="{ox+q*55}" y1="{y-5}" x2="{ox+q*55}" y2="{y+5}" stroke="{RULE}"/>')
  for t in kepler_times(e,w):b.append(f'<circle cx="{ox+220*t:.1f}" cy="{y}" r="5" fill="{V[3]}"/>')
  b.append(text(ox,y+24,'un tour',MUTED,anchor='start',size=11))
  b.append(text(ox+220,y+24,'régulier' if not e else 'pressé, puis retenu',MUTED,anchor='end',size=11))
 b.append(text(270,362,'Les portes restent aux mêmes directions, le tour garde sa durée : seul le moment des notes change (le rubato).',MUTED,size=11))
 return svg(540,376,''.join(b),'Orbite ronde et orbite ovale : mêmes portes, même durée de tour ; sur l’ovale, les notes se resserrent près de l’étoile et s’espacent loin d’elle')

# 3. Several bodies: periods of 3 and 4 bars meet again every 12 bars.
def several():
 b=[];cx,cy=120,140
 for i,(T,col) in enumerate([(3,V[1]),(4,V[2])]):
  r=60*(T/3)**(2/3) # Kepler's third law, as on the screen: farther out = slower
  b.append(f'<circle cx="{cx}" cy="{cy}" r="{r:.1f}" fill="none" stroke="{col}" stroke-opacity=".6" stroke-width="1.5"/>')
  b.append(f'<line x1="{cx}" y1="{cy-r-8:.1f}" x2="{cx}" y2="{cy-r+8:.1f}" stroke="{col}" stroke-width="3" stroke-linecap="round"/>')
  b.append(f'<circle cx="{cx+r*math.cos(-math.pi/2+(.9 if i else 1.4)):.1f}" cy="{cy+r*math.sin(-math.pi/2+(.9 if i else 1.4)):.1f}" r="6" fill="{col}"/>')
 b.append(star(cx,cy))
 b.append(text(cx,240,'deux corps, une porte chacun, en haut',MUTED,size=11))
 b.append(text(250,24,'Un tour de 3 mesures et un tour de 4',anchor='start',weight=600))
 ox,step=250,22
 for i,(T,col,name) in enumerate([(3,V[1],'3 mesures'),(4,V[2],'4 mesures')]):
  y=80+i*44;b.append(text(ox+16,y-12,name,col,anchor='start',size=12,weight=600))
  b.append(f'<line x1="{ox}" y1="{y}" x2="{ox+12*step}" y2="{y}" stroke="{RULE}"/>')
  for m in range(0,13,T):b.append(f'<circle cx="{ox+m*step}" cy="{y}" r="6" fill="{col}"/>')
 y=160
 for m in range(13):b.append(f'<line x1="{ox+m*step}" y1="{y-4}" x2="{ox+m*step}" y2="{y+4}" stroke="{RULE}"/>')
 b.append(f'<line x1="{ox}" y1="{y}" x2="{ox+12*step}" y2="{y}" stroke="{RULE}"/>')
 b.append(text(ox,y+20,'0',MUTED,size=11));b.append(text(ox+12*step+11,y+20,'12 mesures',MUTED,anchor='end',size=11))
 for x in (ox,ox+12*step):b.append(f'<rect x="{x-11}" y="66" width="22" height="100" rx="6" fill="none" stroke="{STAR}" stroke-dasharray="3 3"/>')
 for j,line in enumerate(['Ils ne jouent ensemble que toutes','les 12 mesures (3 × 4) : c’est la','polyrythmie. Alignés avec l’étoile,','ils sont en conjonction.']):b.append(text(ox-11,210+j*17,line,INK,anchor='start',size=12))
 return svg(540,290,''.join(b),'Deux corps de périodes 3 et 4 mesures : leurs notes ne tombent ensemble que toutes les 12 mesures')

for name,f in [('pas-a-orbite',steps_to_orbit),('rond-ovale',round_oval),('plusieurs-corps',several)]:
 (OUT/f'{name}.svg').write_text(f());print('docs/rhizome/schemas/'+name+'.svg')
