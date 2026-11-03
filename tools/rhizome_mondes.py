# SPDX-License-Identifier: GPL-3.0-only
"""Rhizome's factory Mondes, for When Day Chokes The Night: complete v11 patches, read by tools/generate_presets.py.

Three families of four, from dusk to dawn, each showing one side of the orbital sequencer:
 DUSK  resonances: round or nearly round orbits whose periods are simple ratios, heard as slow polyrhythms;
 NIGHT eccentric orbits: rubato (fast near the star, hanging far away), precession and perturbation;
 DAWN  moons and comets: ornaments after each note, rare passages, conjunctions struck as chords.
Then EMPTY SKY, a blank start (no gates, silent).

Each Monde reuses the FDDS writer (tools/fdds_mondes.py) for Monades' part of the patch, then adds Rhizome's: the six
orbits, the comet and conjunctions, the tape, the HALL and Beads. Sounds come from the tone bank. To stay well under
the FM-1's 85 % ceiling, a Monde plays four or five voices, one heavy engine (Rings, the choir) at most."""
from fdds_mondes import fdds,c_patch

PERIOD={4:0,6:1,8:2,12:3,16:4,24:5,32:6,48:7,64:8,96:9,112:10,128:11,192:12} # one turn, in sixteenths
SPIN={1:0,2:1,3:2,4:3,6:4,8:5,12:6,16:7} # one spin of the moons, in sixteenths
PITCH={'ORBIT':0,'MOTIF':1,'ECHO':2}
MOON={'SAME':0,'UP':1,'DOWN':2,'3RD+':3,'3RD-':4,'HARM':5}
COMET={0:0,4:1,8:2,16:3,32:4,64:5} # one turn, in bars
EVENT={'SWELL':0,'FREEZE':1,'BOTH':2}
CONJ={'OFF':0,'ACCENT':1,'CHORD':2}
MEDIA={'DIRTY':0,'CLEAN':1,'DIGITAL':2}
FAMILIES=['Dusk: resonances','Night: eccentric orbits','Dawn: moons and comets','Blank']
SHORT=['DUSK','NIGHT','DAWN','BLANK']

# A silent track: no gates, no hits, muted.
REST=('Soft Sine',48,0,16,dict(level=0,mute=1))
def body(gates,period,ecc=0,phase=0,pitch='ORBIT',moons=0,spin=4,interval='UP',moon_level=60):
 return (gates,PERIOD[period],ecc,phase,PITCH[pitch],moons,SPIN[spin],MOON[interval],moon_level)

# Each Monde: Monades' part as in fdds_mondes (tempo, root, scale, macros DENSITY / MOTION / COLOUR / SPACE, echo
# and room, six tracks: tone, base note, hits, length, overrides; echo= and room= are the track's sends, send= its
# Beads send), then orbits {track: body(...)}, sky (precession, perturbation, comet bars, event, amount,
# conjunction), tape (heads, wow, drive, age), hall (size, predelay, bass, mod, vintage), beads (shape, density,
# random, media); grain = Beads size, position (time), feedback.
MONDES=[
 # ---- DUSK: resonances ----
 dict(name='LAST LIGHT',family=0,tempo=64,root='D',scale='DORIAN',macro=[500,120,480,300],
  echo='1/8 .',delay_fb=380,fx=480,room=620,diffusion=700,grain=(700,420,300),
  tracks=[('Velvet Dusk',38,2,16,dict(level=420,send=100,room=200)),('Soft Tine',62,3,16,dict(level=300,echo=350,room=350,send=250)),
   ('Glass Drop',69,4,16,dict(level=230,echo=300,room=400,send=300)),('Sine Lantern',57,2,16,dict(level=230,attack=500,decay=850,room=500,send=200)),
   ('Mellow Dusk',50,1,16,dict(level=220,attack=650,decay=900,room=500,send=150)),REST],
  orbits={0:body(2,128),1:body(3,48,ecc=150),2:body(4,64,ecc=200,phase=6),3:body(2,96,ecc=100,phase=12),4:body(1,192,phase=18)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(3,150,100,200),hall=(750,30,550,350,0),beads=(600,800,250,'DIGITAL')),
 dict(name='AMBER HOUR',family=0,tempo=72,root='A',scale='MINOR PENT',macro=[500,100,520,260],
  echo='1/8 .',delay_fb=450,fx=460,room=560,grain=(600,350,250),
  tracks=[('Amber Bass',33,2,16,dict(level=430,send=80,room=150)),('Copper Bell',69,4,16,dict(level=240,echo=400,room=350,send=250)),
   ('Amber Keys',64,5,16,dict(level=250,echo=300,room=300,send=250)),('Hollow Chime',76,3,16,dict(level=190,echo=450,room=400,send=300)),
   REST,REST],
  orbits={0:body(2,128),1:body(4,16,ecc=80),2:body(5,32,ecc=120,phase=4),3:body(3,64,ecc=150,phase=10)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(1,120,150,150),hall=(650,25,500,300,0),beads=(500,820,200,'DIGITAL')),
 dict(name='LONG SHADOWS',family=0,tempo=60,root='F',scale='LYDIAN',macro=[500,80,460,280],
  echo='1/4',delay_fb=320,fx=470,room=650,diffusion=750,grain=(750,500,300),
  tracks=[('Night Floor',29,3,16,dict(level=400,send=80,room=200)),('Dream Marimba',65,5,16,dict(level=240,echo=250,room=350,send=250)),
   ('Pure Water',72,5,16,dict(level=220,echo=250,room=400,send=250)),('Moon Tone',60,5,16,dict(level=230,echo=250,room=400,send=250)),
   REST,REST],
  orbits={0:body(3,192),1:body(5,96,ecc=60),2:body(5,112,ecc=60,phase=8),3:body(5,128,ecc=60,phase=16)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(2,100,100,150),hall=(800,40,600,350,0),beads=(650,780,300,'DIGITAL')),
 dict(name='SLOW EMBERS',family=0,tempo=54,root='E',scale='MINOR',macro=[500,60,440,320],
  echo='1/8 .',delay_fb=300,fx=500,room=720,diffusion=800,grain=(800,450,350),
  tracks=[('Velvet Ember',28,1,16,dict(level=400,send=80,room=250)),('Hollow Flute',59,6,16,dict(level=260,attack=200,echo=300,room=400,send=250)),
   ('Soft Sine',71,6,16,dict(level=200,attack=150,echo=300,room=500,send=300)),('Slow Halo',52,1,16,dict(level=200,attack=800,decay=950,room=500,send=200)),
   REST,REST],
  phrase={1:[5,-1,7,-1,8,-1,7,-1,5,-1,3,-1,5,-1,-1,-1]}, # E F# G F# E D E, from B3
  orbits={0:body(1,192),1:body(6,64,pitch='MOTIF'),2:body(6,64,phase=2,pitch='ECHO'),3:body(1,192,phase=12)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(1,80,80,120),hall=(850,45,650,300,0),beads=(700,760,300,'CLEAN')),
 # ---- NIGHT: eccentric orbits ----
 dict(name='NIGHT SWALLOWS',family=1,tempo=58,root='C',scale='PHRYGIAN',macro=[500,80,420,420],
  echo='1/8 .',delay_fb=420,fx=500,room=700,diffusion=750,grain=(700,550,400),
  tracks=[('Deep Root',36,2,16,dict(level=400,send=120,room=200)),('Deep Bloom',60,4,16,dict(level=250,echo=300,room=400,send=300)),
   ('Low Orbit',55,3,16,dict(level=240,echo=350,room=400,send=300)),('Night Voices',48,1,16,dict(level=190,attack=700,decay=950,room=500,send=250)),
   REST,REST],
  orbits={0:body(2,64,ecc=400),1:body(4,48,ecc=700,phase=3),2:body(3,96,ecc=650,phase=14),3:body(1,192,ecc=300)},
  sky=(25,10,0,'SWELL',60,'ACCENT'),tape=(3,250,150,350),hall=(800,35,600,450,0),beads=(650,840,350,'DIRTY')),
 dict(name='BLACK ORCHARD',family=1,tempo=66,root='G',scale='KUMOI',macro=[500,120,460,380],
  echo='1/8',delay_fb=400,fx=480,room=620,grain=(600,400,350),
  tracks=[('Velvet Moss',31,2,16,dict(level=400,send=100,room=200)),('Clay Bell',67,4,16,dict(level=240,echo=300,room=350,send=280)),
   ('Muted Pluck',62,5,16,dict(level=250,echo=350,room=300,send=280)),('Mellow Moss',55,2,16,dict(level=220,attack=400,room=450,send=220)),
   REST,REST],
  orbits={0:body(2,96,ecc=350),1:body(4,32,ecc=550,phase=5),2:body(5,48,ecc=300,phase=11),3:body(2,128,ecc=620,phase=19)},
  sky=(15,45,0,'SWELL',60,'ACCENT'),tape=(5,300,200,500),hall=(700,30,550,400,0),beads=(450,860,450,'DIRTY')),
 dict(name='SLEEPLESS TIDE',family=1,tempo=50,root='D',scale='JUST MINOR',macro=[500,60,440,360],
  echo='1/4',delay_fb=350,fx=520,room=780,diffusion=850,grain=(850,500,350),
  tracks=[('Slow Earth',26,2,16,dict(level=400,send=100,room=250)),('Sine Pearl',74,3,16,dict(level=220,echo=400,room=450,send=300)),
   ('Warm Strings',62,2,16,dict(level=230,attack=600,decay=900,room=500,send=250)),('Cedar Pad',50,2,16,dict(level=220,attack=700,decay=950,room=500,send=250)),
   REST,REST],
  orbits={0:body(2,192,ecc=500),1:body(3,48,ecc=750,phase=7),2:body(2,96,ecc=600,phase=15),3:body(2,128,ecc=800,phase=2)},
  sky=(60,5,0,'SWELL',60,'ACCENT'),tape=(4,200,100,250),hall=(900,50,650,450,0),beads=(750,790,300,'CLEAN')),
 dict(name='NO STARS',family=1,tempo=46,root='B',scale='LOCRIAN',macro=[500,40,400,460],
  echo='1/4',delay_fb=450,fx=540,room=880,diffusion=900,grain=(900,600,450),
  tracks=[('Low Current',35,1,16,dict(level=380,attack=300,send=150,room=300)),('Slow Glass',71,2,16,dict(level=220,echo=450,room=500,send=350)),
   ('High Mist',66,1,16,dict(level=180,attack=600,decay=950,room=550,send=300)),REST,REST,REST],
  orbits={0:body(1,128,ecc=300),1:body(2,112,ecc=820,phase=9),2:body(1,96,ecc=600,phase=20)},
  sky=(30,20,32,'FREEZE',70,'OFF'),tape=(4,250,80,400),hall=(950,60,700,500,0),beads=(800,830,400,'DIRTY')),
 # ---- DAWN: moons and comets ----
 dict(name='FIRST BIRD',family=2,tempo=76,root='G',scale='MAJOR PENT',macro=[500,100,540,260],
  echo='1/8 .',delay_fb=380,fx=460,room=560,grain=(600,350,250),
  tracks=[('Felt Octave',31,2,16,dict(level=400,send=80,room=150)),('Sine Pearl',74,3,16,dict(level=220,echo=350,room=350,send=250)),
   ('Glass Drop',67,4,16,dict(level=230,echo=300,room=350,send=250)),('Major Dawn',55,1,16,dict(level=190,attack=700,decay=950,room=450,send=200)),
   REST,REST],
  orbits={0:body(2,64),1:body(3,48,ecc=200,moons=2,spin=3,interval='UP',moon_level=55),2:body(4,32,ecc=150,phase=6,moons=3,spin=4,interval='3RD+',moon_level=50),3:body(1,192)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(3,120,100,150),hall=(650,25,500,300,0),beads=(500,800,200,'DIGITAL')),
 dict(name='PALE COMET',family=2,tempo=62,root='E',scale='LYDIAN',macro=[500,80,500,300],
  echo='1/8 .',delay_fb=400,fx=480,room=640,diffusion=750,grain=(700,450,300),
  tracks=[('Velvet Lagoon',28,2,16,dict(level=400,send=80,room=200)),('Hollow Chime',71,4,16,dict(level=220,echo=350,room=400,send=280)),
   ('Amber Pad',59,2,16,dict(level=210,attack=600,decay=900,room=500,send=220)),('Linen Strings',64,2,16,dict(level=220,attack=500,decay=900,room=450,send=220)),
   REST,REST],
  orbits={0:body(2,128),1:body(4,48,ecc=250,moons=1,spin=6,interval='HARM',moon_level=45),2:body(2,128,ecc=100,phase=12),3:body(2,96,ecc=150,phase=6)},
  sky=(0,0,16,'SWELL',85,'ACCENT'),tape=(3,150,100,200),hall=(800,35,600,350,0),beads=(650,800,250,'DIGITAL')),
 dict(name='CHOKED DAWN',family=2,tempo=56,root='F#',scale='DORIAN',macro=[500,80,460,360],
  echo='1/8 .',delay_fb=550,fx=500,room=700,diffusion=800,grain=(750,450,350),
  tracks=[('Velvet Haze',30,2,16,dict(level=400,send=100,room=200)),('Warm Fifth',54,3,16,dict(level=230,attack=300,echo=400,room=400,send=280)),
   ('Dusky Organ',61,3,16,dict(level=220,attack=250,echo=400,room=400,send=280)),('Mellow Haze',66,3,16,dict(level=210,attack=200,echo=400,room=450,send=280)),
   REST,REST],
  orbits={0:body(2,128,ecc=200),1:body(3,32,ecc=300),2:body(3,48,ecc=300,phase=8),3:body(3,64,ecc=300,phase=16)},
  sky=(10,10,8,'BOTH',50,'CHORD'),tape=(7,450,400,700),hall=(800,40,600,400,0),beads=(650,820,350,'CLEAN')),
 dict(name='DAY BREAKS',family=2,tempo=84,root='D',scale='MIXOLYDIAN',macro=[500,100,560,240],
  echo='1/8 .',delay_fb=380,fx=450,room=560,grain=(600,350,250),
  tracks=[('Clear Foundation',38,3,16,dict(level=400,send=80,room=150)),('Morning Ribbon',62,6,16,dict(level=230,echo=350,room=350,send=250)),
   ('Felt Keys',69,5,16,dict(level=230,echo=300,room=300,send=250)),('Sunlit Voices',57,1,16,dict(level=180,attack=700,decay=950,room=450,send=200)),
   REST,REST],
  phrase={1:[0,-1,4,-1,7,-1,9,-1,7,-1,4,-1,2,-1,-1,-1]},
  orbits={0:body(3,64),1:body(6,64,pitch='MOTIF',moons=2,spin=2,interval='HARM',moon_level=45),2:body(5,32,ecc=200,phase=3,moons=1,spin=3,interval='UP',moon_level=50),3:body(1,192)},
  sky=(0,0,4,'FREEZE',40,'CHORD'),tape=(3,120,100,150),hall=(650,25,500,300,0),beads=(500,800,200,'DIGITAL')),
]
# A blank start, as Monades' blank: no gates, no hits, the effects returned at zero.
BLANK=dict(name='EMPTY SKY',family=3,tempo=64,root='C',scale='MINOR',macro=[500,0,500,0],fx=0,room=600,
 tracks=[('Soft Tine',48+i*7 if i<4 else 60,0,16,dict(level=300,room=0)) for i in range(6)],orbits={},
 phrase={i:[0 if j%4==0 else 7 if j%4==2 else -1 for j in range(16)] for i in range(6)}, # as Monades' blank: room for Marbles
 sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(1,0,0,0),hall=(700,20,550,300,0),beads=(500,800,200,'DIGITAL'))

# Levels: every Monde near -30 dB RMS over a minute (NO STARS, sparse by design, near -36), measured by
# tests/next_features.cpp; the tracks' levels above are the balance inside a Monde, these the Monde's own.
GAIN={'LAST LIGHT':.77,'AMBER HOUR':.88,'LONG SHADOWS':1.5,'SLOW EMBERS':1.23,'NIGHT SWALLOWS':1.57,'BLACK ORCHARD':1.32,
 'SLEEPLESS TIDE':1.6,'NO STARS':3.0,'FIRST BIRD':1.78,'PALE COMET':1.23,'CHOKED DAWN':.88,'DAY BREAKS':1.33}

def rhizome(tones,fields):
 """Catalog entries: the FDDS writer's v10 part, plus 'v11', Rhizome's part."""
 out=fdds(tones,fields,0,mondes=MONDES+[BLANK],seed=0x52485A00)
 for b,m in zip(out,MONDES+[BLANK]):
  for i,tr in enumerate(b['tracks']):
   tr['prob']=1000 # the seed is locked: a gate that misses would miss on every turn
   tr['level']=min(1000,round(tr['level']*GAIN.get(m['name'],1)))
   if i in m['orbits']:tr['hits']=0 # a body plays its gates, not a rhythm
  prec,pert,comet,event,amount,conj=m['sky']
  b['v11']=dict(orbit=[m['orbits'].get(i,body(0,16)) for i in range(6)],sky=[prec,pert,COMET[comet],EVENT[event],amount,CONJ[conj],0],
   tape=list(m['tape']),hall=list(m['hall']),beads=list(m['beads'][:3])+[MEDIA[m['beads'][3]]])
  b['v10']=False;b['v11ok']=True
 return out

def c_patch11(b,fields):
 """The C initializer of a complete v11 md_patch: the v10 part, then the orbits and Rhizome's effects."""
 j=lambda xs:','.join(map(str,xs))
 v=b['v11'];head=c_patch(b,fields).split(',',2);head[1]='11';head=','.join(head) # magic, version, seed
 return head+',{'+','.join('{'+j(o)+'}' for o in v['orbit'])+'},'+j(v['sky']+v['tape']+v['hall']+v['beads'])
