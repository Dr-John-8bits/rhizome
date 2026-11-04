# SPDX-License-Identifier: GPL-3.0-only
"""Rhizome's factory Mondes, for When Day Chokes The Night: complete v12 patches, read by tools/generate_presets.py.

Three families of four, from dusk to dawn, each showing one side of the orbital sequencer:
 DUSK  resonances: round or nearly round orbits whose periods are simple ratios, heard as slow polyrhythms;
 NIGHT eccentric orbits: rubato (fast near the star, hanging far away), precession and perturbation;
 DAWN  moons and comets: ornaments after each note, rare passages, conjunctions struck as chords;
 DEEP  long forms: LFOs, generators, the form, real planetary systems, gamakas and free periods, Mondes that keep
       changing for minutes (not all in one).
Then EMPTY SKY, a blank start (no gates, silent).

Each Monde reuses the FDDS writer (tools/fdds_mondes.py) for Monades' part of the patch, then adds Rhizome's: the six
orbits, the comet and conjunctions, the tape, the HALL and Beads. Sounds come from the tone bank. To stay well under
the FM-1's 85 % ceiling, a Monde plays four or five of Rhizome's voices (tools/rhizome_tones.py)."""
from fdds_mondes import fdds,c_patch

PERIOD={4:0,6:1,8:2,12:3,16:4,24:5,32:6,48:7,64:8,96:9,112:10,128:11,192:12, # one turn, in sixteenths
 '5.1s':13,'7.3s':14,'11.6s':15,'17.8s':16,'20.1s':17,'23.5s':18,'29.9s':19,'31.8s':20} # free periods, in seconds
SPIN={1:0,2:1,3:2,4:3,6:4,8:5,12:6,16:7} # one spin of the moons, in sixteenths
PITCH={'ORBIT':0,'MOTIF':1,'ECHO':2}
MOON={'SAME':0,'UP':1,'DOWN':2,'3RD+':3,'3RD-':4,'HARM':5}
COMET={0:0,4:1,8:2,16:3,32:4,64:5} # one turn, in bars
EVENT={'SWELL':0,'FREEZE':1,'BOTH':2,'FILTER':3}
CONJ={'OFF':0,'ACCENT':1,'CHORD':2}
MEDIA={'DIRTY':0,'CLEAN':1,'DIGITAL':2}
FAMILIES=['Dusk: resonances','Night: eccentric orbits','Dawn: moons and comets','Deep: long forms','Blank']
SHORT=['DUSK','NIGHT','DAWN','DEEP','BLANK']
SYSTEM={'OWN':0,'JUPITER':1,'KEPLER-223':2,'TRAPPIST-1':3,'PLUTO':4}

# A silent track: no gates, no hits, muted.
REST=('Soft Table',48,0,16,dict(level=0,mute=1))
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
  tracks=[('Low Terrain',38,2,16,dict(level=420,send=100,room=200)),('DX Vibe',62,3,16,dict(level=300,echo=350,room=350,send=250)),
   ('Glass Rows',69,4,16,dict(level=230,echo=300,room=400,send=300)),('DX Planets',57,2,16,dict(level=230,attack=500,decay=850,room=500,send=200)),
   ('Solina Dusk',50,1,16,dict(level=220,attack=650,decay=900,room=500,send=150)),REST],
  orbits={0:body(2,128),1:body(3,48,ecc=150),2:body(4,64,ecc=200,phase=6),3:body(2,96,ecc=100,phase=12),4:body(1,192,phase=18)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(3,150,100,200),hall=(750,30,550,350,0),beads=(600,800,250,'DIGITAL')),
 dict(name='AMBER HOUR',family=0,tempo=72,root='A',scale='MINOR PENT',macro=[500,100,520,260],
  echo='1/8 .',delay_fb=450,fx=460,room=560,grain=(600,350,250),
  tracks=[('Low Phase',33,2,16,dict(level=430,send=80,room=150)),('DX Tub Bells',69,4,16,dict(level=240,echo=400,room=350,send=250)),
   ('DX E.Piano',64,5,16,dict(level=250,echo=300,room=300,send=250)),('Bell Table',76,3,16,dict(level=190,echo=450,room=400,send=300)),
   REST,REST],
  orbits={0:body(2,128),1:body(4,16,ecc=80),2:body(5,32,ecc=120,phase=4),3:body(3,64,ecc=150,phase=10)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(1,120,150,150),hall=(650,25,500,300,0),beads=(500,820,200,'DIGITAL')),
 dict(name='LONG SHADOWS',family=0,tempo=60,root='F',scale='LYDIAN',macro=[500,80,460,280],
  echo='1/4',delay_fb=320,fx=470,room=650,diffusion=750,grain=(750,500,300),
  tracks=[('Low Table',29,3,16,dict(level=400,send=80,room=200)),('Bell Phase',65,5,16,dict(level=240,echo=250,room=350,send=250)),
   ('Glass Phase',72,5,16,dict(level=220,echo=250,room=400,send=250)),('DX Winter Rhodes',60,5,16,dict(level=230,echo=250,room=400,send=250)),
   REST,REST],
  orbits={0:body(3,192),1:body(5,96,ecc=60),2:body(5,112,ecc=60,phase=8),3:body(5,128,ecc=60,phase=16)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(2,100,100,150),hall=(800,40,600,350,0),beads=(650,780,300,'DIGITAL')),
 dict(name='SLOW EMBERS',family=0,tempo=54,root='E',scale='MINOR',macro=[500,60,440,320],
  echo='1/8 .',delay_fb=300,fx=500,room=720,diffusion=800,grain=(800,450,350),
  tracks=[('Valley Hum',28,1,16,dict(level=400,send=80,room=250)),('Breath Vowel',59,6,16,dict(level=260,attack=200,echo=300,room=400,send=250)),
   ('Soft Phase',71,6,16,dict(level=200,attack=150,echo=300,room=500,send=300)),('Sus Choir',52,1,16,dict(level=200,attack=800,decay=950,room=500,send=200)),
   REST,REST],
  phrase={1:[5,-1,7,-1,8,-1,7,-1,5,-1,3,-1,5,-1,-1,-1]}, # E F# G F# E D E, from B3
  orbits={0:body(1,192),1:body(6,64,pitch='MOTIF'),2:body(6,64,phase=2,pitch='ECHO'),3:body(1,192,phase=12)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(1,80,80,120),hall=(850,45,650,300,0),beads=(700,760,300,'CLEAN')),
 # ---- NIGHT: eccentric orbits ----
 dict(name='NIGHT SWALLOWS',family=1,tempo=58,root='C',scale='PHRYGIAN',macro=[500,80,420,420],
  echo='1/8 .',delay_fb=420,fx=500,room=700,diffusion=750,grain=(700,550,400),
  tracks=[('Low Hum',36,2,16,dict(level=400,send=120,room=200)),('DX Soundtrack',60,4,16,dict(level=250,echo=300,room=400,send=300)),
   ('Night Table',55,3,16,dict(level=240,echo=350,room=400,send=300)),('Night Voice',48,1,16,dict(level=190,attack=700,decay=950,room=500,send=250)),
   REST,REST],
  orbits={0:body(2,64,ecc=400),1:body(4,48,ecc=700,phase=3),2:body(3,96,ecc=650,phase=14),3:body(1,192,ecc=300)},
  sky=(25,10,0,'SWELL',60,'ACCENT'),tape=(3,250,150,350),hall=(800,35,600,450,0),beads=(650,840,350,'DIRTY')),
 dict(name='BLACK ORCHARD',family=1,tempo=66,root='G',scale='KUMOI',macro=[500,120,460,380],
  echo='1/8',delay_fb=400,fx=480,room=620,grain=(600,400,350),
  tracks=[('Deep Crater',31,2,16,dict(level=400,send=100,room=200)),('DX Fairlight',67,4,16,dict(level=240,echo=300,room=350,send=280)),
   ('Warm Reed',62,5,16,dict(level=250,echo=350,room=300,send=280)),('Moss Field',55,2,16,dict(level=220,attack=400,room=450,send=220)),
   REST,REST],
  orbits={0:body(2,96,ecc=350),1:body(4,32,ecc=550,phase=5),2:body(5,48,ecc=300,phase=11),3:body(2,128,ecc=620,phase=19)},
  sky=(15,45,0,'SWELL',60,'ACCENT'),tape=(5,300,200,500),hall=(700,30,550,400,0),beads=(450,860,450,'DIRTY')),
 dict(name='SLEEPLESS TIDE',family=1,tempo=50,root='D',scale='JUST MINOR',macro=[500,60,440,360],
  echo='1/4',delay_fb=350,fx=520,room=780,diffusion=850,grain=(850,500,350),
  tracks=[('Low Strings',26,2,16,dict(level=400,send=100,room=250)),('DX Wineglass',74,3,16,dict(level=220,echo=400,room=450,send=300)),
   ('Ensemble Haze',62,2,16,dict(level=230,attack=600,decay=900,room=500,send=250)),('DX Cirrus',50,2,16,dict(level=220,attack=700,decay=950,room=500,send=250)),
   REST,REST],
  orbits={0:body(2,192,ecc=500),1:body(3,48,ecc=750,phase=7),2:body(2,96,ecc=600,phase=15),3:body(2,128,ecc=800,phase=2)},
  sky=(60,5,0,'SWELL',60,'ACCENT'),tape=(4,200,100,250),hall=(900,50,650,450,0),beads=(750,790,300,'CLEAN')),
 dict(name='NO STARS',family=1,tempo=46,root='B',scale='LOCRIAN',macro=[500,40,400,460],
  echo='1/4',delay_fb=450,fx=540,room=880,diffusion=900,grain=(900,600,450),
  tracks=[('Drone Hive',35,1,16,dict(level=380,attack=300,send=150,room=300)),('Glass Swarm',71,2,16,dict(level=220,echo=450,room=500,send=350)),
   ('Ghost Rows',66,1,16,dict(level=180,attack=600,decay=950,room=550,send=300)),REST,REST,REST],
  orbits={0:body(1,128,ecc=300),1:body(2,112,ecc=820,phase=9),2:body(1,96,ecc=600,phase=20)},
  sky=(30,20,32,'FREEZE',70,'OFF'),tape=(4,250,80,400),hall=(950,60,700,500,0),beads=(800,830,400,'DIRTY')),
 # ---- DAWN: moons and comets ----
 dict(name='FIRST BIRD',family=2,tempo=76,root='G',scale='MAJOR PENT',macro=[500,100,540,260],
  echo='1/8 .',delay_fb=380,fx=460,room=560,grain=(600,350,250),
  tracks=[('Open Octaves',31,2,16,dict(level=400,send=80,room=150)),('DX Wineglass',74,3,16,dict(level=220,echo=350,room=350,send=250)),
   ('Glass Rows',67,4,16,dict(level=230,echo=300,room=350,send=250)),('Major Nine',55,1,16,dict(level=190,attack=700,decay=950,room=450,send=200)),
   REST,REST],
  orbits={0:body(2,64),1:body(3,48,ecc=200,moons=2,spin=3,interval='UP',moon_level=55),2:body(4,32,ecc=150,phase=6,moons=3,spin=4,interval='3RD+',moon_level=50),3:body(1,192)},
  sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(3,120,100,150),hall=(650,25,500,300,0),beads=(500,800,200,'DIGITAL')),
 dict(name='PALE COMET',family=2,tempo=62,root='E',scale='LYDIAN',macro=[500,80,500,300],
  echo='1/8 .',delay_fb=400,fx=480,room=640,diffusion=750,grain=(700,450,300),
  tracks=[('Low Hive',28,2,16,dict(level=400,send=80,room=200)),('Bell Table',71,4,16,dict(level=220,echo=350,room=400,send=280)),
   ('DX Etherial',59,2,16,dict(level=210,attack=600,decay=900,room=500,send=220)),('Pale Orchestra',64,2,16,dict(level=220,attack=500,decay=900,room=450,send=220)),
   REST,REST],
  orbits={0:body(2,128),1:body(4,48,ecc=250,moons=1,spin=6,interval='HARM',moon_level=45),2:body(2,128,ecc=100,phase=12),3:body(2,96,ecc=150,phase=6)},
  sky=(0,0,16,'SWELL',85,'ACCENT'),tape=(3,150,100,200),hall=(800,35,600,350,0),beads=(650,800,250,'DIGITAL')),
 dict(name='CHOKED DAWN',family=2,tempo=56,root='F#',scale='DORIAN',macro=[500,80,460,360],
  echo='1/8 .',delay_fb=550,fx=500,room=700,diffusion=800,grain=(750,450,350),
  tracks=[('Dark CZ',30,2,16,dict(level=400,send=100,room=200)),('Organ Fifths',54,3,16,dict(level=230,attack=300,echo=400,room=400,send=280)),
   ('Night Ensemble',61,3,16,dict(level=220,attack=250,echo=400,room=400,send=280)),('Choir Ah',66,3,16,dict(level=210,attack=200,echo=400,room=450,send=280)),
   REST,REST],
  orbits={0:body(2,128,ecc=200),1:body(3,32,ecc=300),2:body(3,48,ecc=300,phase=8),3:body(3,64,ecc=300,phase=16)},
  sky=(10,10,8,'BOTH',50,'CHORD'),tape=(7,450,400,700),hall=(800,40,600,400,0),beads=(650,820,350,'CLEAN')),
 dict(name='DAY BREAKS',family=2,tempo=84,root='D',scale='MIXOLYDIAN',macro=[500,100,560,240],
  echo='1/8 .',delay_fb=380,fx=450,room=560,grain=(600,350,250),
  tracks=[('Low Phase',38,3,16,dict(level=400,send=80,room=150)),('Pale Lead',62,6,16,dict(level=230,echo=350,room=350,send=250)),
   ('Bright CZ',69,5,16,dict(level=230,echo=300,room=300,send=250)),('High Major',57,1,16,dict(level=180,attack=700,decay=950,room=450,send=200)),
   REST,REST],
  phrase={1:[0,-1,4,-1,7,-1,9,-1,7,-1,4,-1,2,-1,-1,-1]},
  orbits={0:body(3,64),1:body(6,64,pitch='MOTIF',moons=2,spin=2,interval='HARM',moon_level=45),2:body(5,32,ecc=200,phase=3,moons=1,spin=3,interval='UP',moon_level=50),3:body(1,192)},
  sky=(0,0,4,'FREEZE',40,'CHORD'),tape=(3,120,100,150),hall=(650,25,500,300,0),beads=(500,800,200,'DIGITAL')),

 # ---- DEEP: long forms ----
 # The showcase: few steps (loops of 4 to 12 sixteenths), every kind of note (ORBIT, MOTIF mutated by its generator,
 # ECHO), moons, two free periods that never fall in step, precession, perturbation, a comet, chords, slow LFOs, the form.
 dict(name='COLD IN THE DARK',family=3,tempo=72,root='D',scale='MINOR',macro=[500,60,460,320],
  echo='1/8 .',delay_fb=420,fx=480,room=700,diffusion=800,grain=(700,450,350),form=(600,2,2),
  tracks=[('Low Hum',38,1,16,dict(level=380,send=100,room=150)),('Glass Rows',62,1,16,dict(level=230,echo=350,room=400,send=280)),
   ('DX Vibe',69,2,16,dict(level=210,echo=300,room=450,send=250)),('Ensemble Haze',57,1,16,dict(level=200,attack=650,decay=950,room=500,send=220)),
   ('Soft Swarm',45,1,16,dict(level=180,attack=700,decay=950,room=500,send=250)),REST],
  phrase={1:[0,-1,3,-1,7,-1,10,-1,12,-1,10,-1,7,-1,5,-1]},
  orbits={0:body(1,12),1:body(1,4,ecc=300,pitch='MOTIF',moons=2,spin=3,interval='UP',moon_level=45),2:body(2,6,ecc=450,phase=6,pitch='ECHO'),
   3:body(1,'17.8s',phase=3),4:body(1,'29.9s',phase=15)},
  lfo={(1,0):('FREE SLOW','SINE','TONE',500),(3,0):('8 BARS','S&H','HARMONICS',300),(2,0):('4 BARS','TRIANGLE','CUTOFF',350)},
  gen={1:('MOTIF',700,8,7)},
  sky=(40,25,16,'BOTH',50,'CHORD'),tape=(3,200,150,250),hall=(800,40,600,450,0),beads=(650,820,300,'CLEAN')),
 # Kepler-223 as measured (3:4:6:8, its eccentricities), the wavetables' colour turning with a slow LFO; every 16 bars a
 # comet whose FILTER opens the whole sky as it grazes the star.
 dict(name='KEPLER NIGHT',family=3,tempo=60,root='A',scale='DORIAN',macro=[500,60,460,320],
  echo='1/4',delay_fb=380,fx=480,room=720,diffusion=800,grain=(750,500,300),
  tracks=[('Night Table',57,4,16,dict(level=230,echo=300,room=450,send=250)),('DX Cirrus',64,4,16,dict(level=210,room=500,send=250)),
   ('Pale Orchestra',52,4,16,dict(level=200,attack=500,decay=930,room=500,send=220)),('Bell Phase',69,4,16,dict(level=210,echo=350,room=450,send=250)),
   ('Drone Hive',33,1,16,dict(level=360,attack=800,decay=960,room=250,send=100)),REST],
  phrase={0:[0,-1,2,-1,3,-1,7,-1,5,-1,3,-1,2,-1,-1,-1]},
  orbits={0:body(4,48,ecc=78,pitch='MOTIF'),1:body(4,64,ecc=150),2:body(4,96,ecc=37),3:body(4,128,ecc=51),4:body(1,'31.8s')},
  lfo={(0,0):('FREE SLOW','SINE','TONE',450),(0,1):('8 BARS','TRIANGLE','SHAPE',350),(2,0):('4 BARS','SINE','CUTOFF',300)},
  sky=(10,0,16,'FILTER',55,'ACCENT','KEPLER-223'),tape=(3,150,100,200),hall=(850,45,650,400,0),beads=(700,800,250,'DIGITAL')),
 # TRAPPIST-1's resonant chain (2:3:4:6), its notes drawn by generators that remember (deja-vu) and slowly let go.
 dict(name='TRAPPIST DUST',family=3,tempo=66,root='E',scale='HIRAJOSHI',macro=[500,40,480,360],
  echo='1/8 .',delay_fb=450,fx=500,room=680,grain=(650,450,400),
  tracks=[('Wild Swarm',52,4,16,dict(level=200,room=400,send=300)),('Glass Vowel',64,4,16,dict(level=220,echo=300,room=450,send=280)),
   ('Six Nine',57,4,16,dict(level=200,attack=300,room=450,send=250)),('Dust Table',69,4,16,dict(level=200,echo=350,room=450,send=280)),
   ('Low Terrain',40,1,16,dict(level=360,send=100,room=200)),REST],
  orbits={0:body(4,32,ecc=5),1:body(4,48,ecc=10,phase=4),2:body(4,64,ecc=3,phase=8),3:body(4,96,ecc=4,phase=12),4:body(1,192)},
  gen={1:('RANDOM',800,8,7),3:('MARKOV',600,8,12)},
  lfo={(0,0):('FREE SLOW','SMOOTH','HARMONICS',400)},
  sky=(0,10,0,'SWELL',60,'ACCENT','TRAPPIST-1'),tape=(5,250,150,350),hall=(750,35,550,400,0),beads=(600,840,400,'DIRTY')),
 # A raga at night (Bhairavi): a drone, a breath that sings the phrase with meend, a reed that sways (andolan), bells
 # with kan; the pivot notes draw the generated ones.
 dict(name='RAGA OF ASH',family=3,tempo=52,root='D',scale='BHAIRAVI',macro=[500,40,440,340],vadi=5,samvadi=0,
  echo='1/4',delay_fb=350,fx=500,room=760,diffusion=850,grain=(800,500,350),
  tracks=[('Throat Drone',38,1,16,dict(level=360,send=100,room=250)),('Breath Vowel',62,6,16,dict(level=250,attack=150,echo=300,room=450,send=250)),
   ('Warm Reed',57,3,16,dict(level=220,attack=100,echo=300,room=450,send=250)),('DX Tub Bells',74,2,16,dict(level=190,echo=400,room=500,send=300)),
   ('Organ Fifths',50,1,16,dict(level=200,attack=800,decay=960,room=450,send=200)),REST],
  phrase={1:[0,-1,1,-1,5,-1,-1,-1,7,-1,5,-1,3,-1,1,-1]},
  orbits={0:body(1,192),1:body(6,64,ecc=250,pitch='MOTIF'),2:body(3,96,ecc=400,phase=8),3:body(2,48,ecc=600,phase=16),4:body(1,128,phase=12)},
  gamaka={1:('MEEND',700),2:('ANDOLAN',600),3:('KAN',500)},
  gen={2:('MARKOV',500,8,7)},
  sky=(15,10,0,'SWELL',60,'ACCENT'),tape=(1,150,100,200),hall=(900,50,650,450,0),beads=(750,780,300,'CLEAN')),
 # The form: calm, build, peak, release, sixteen bars each, tracks falling silent and coming back; slow LFOs open and
 # close the filters and move the voices across.
 dict(name='SLOW TIDES',family=3,tempo=58,root='F',scale='MIXOLYDIAN',macro=[500,60,460,340],form=(900,3,3),
  echo='1/8 .',delay_fb=400,fx=500,room=740,diffusion=800,grain=(750,450,350),
  tracks=[('Low Strings',41,2,16,dict(level=380,send=100,room=200)),('Warm Ensemble',65,3,16,dict(level=210,attack=500,room=450,send=250)),
   ('Mist Basin',60,3,16,dict(level=210,echo=300,room=450,send=280)),('DX Soundtrack',72,2,16,dict(level=200,echo=350,room=500,send=280)),
   ('Breath Table',53,2,16,dict(level=200,attack=400,room=450,send=250)),REST],
  orbits={0:body(2,128,ecc=200),1:body(3,64,ecc=350,phase=4),2:body(3,96,ecc=500,phase=10),3:body(2,48,ecc=600,phase=18),4:body(2,112,ecc=250,phase=14)},
  lfo={(1,0):('8 BARS','SINE','CUTOFF',500),(2,0):('4 BARS','TRIANGLE','PAN',600),(3,0):('FREE SLOW','SINE','ROOM SEND',500),(4,0):('8 BARS','SMOOTH','TONE',400)},
  sky=(30,10,0,'SWELL',60,'ACCENT'),tape=(3,200,100,250),hall=(850,45,600,450,0),beads=(700,800,300,'CLEAN')),
 # Five free periods, in seconds whatever the tempo, each with one gate: the loops of Music for Airports 2/1 (Brian
 # Eno's tape lengths), five voices that never fall together, so the music never repeats.
 dict(name='LOOPS OF ASH',family=3,tempo=60,root='F',scale='MINOR',macro=[500,0,460,300],
  echo='1/4',delay_fb=300,fx=520,room=800,diffusion=850,grain=(800,500,300),
  tracks=[('Choir Ah',53,1,16,dict(level=230,attack=600,decay=920,room=500,send=200)),('Choir Oo',56,1,16,dict(level=220,attack=600,decay=920,room=500,send=200)),
   ('Night Voice',60,1,16,dict(level=220,attack=600,decay=920,room=500,send=200)),('Solina Dusk',61,1,16,dict(level=200,attack=700,decay=940,room=500,send=200)),
   ('Distant Strings',63,1,16,dict(level=200,attack=700,decay=940,room=500,send=200)),REST],
  orbits={0:body(1,'17.8s'),1:body(1,'20.1s',phase=5),2:body(1,'23.5s',phase=10),3:body(1,'29.9s',phase=15),4:body(1,'31.8s',phase=20)},
  sky=(0,0,0,'SWELL',60,'OFF'),tape=(4,100,50,150),hall=(900,60,650,400,0),beads=(750,780,250,'CLEAN')),
]
# A blank start, as Monades' blank: no gates, no hits, the effects returned at zero.
BLANK=dict(name='EMPTY SKY',family=4,tempo=64,root='C',scale='MINOR',macro=[500,0,500,0],fx=0,room=600,
 tracks=[('DX Vibe',48+i*7 if i<4 else 60,0,16,dict(level=300,room=0)) for i in range(6)],orbits={},
 phrase={i:[0 if j%4==0 else 7 if j%4==2 else -1 for j in range(16)] for i in range(6)}, # as Monades' blank: room for Marbles
 sky=(0,0,0,'SWELL',60,'ACCENT'),tape=(1,0,0,0),hall=(700,20,550,300,0),beads=(500,800,200,'DIGITAL'))

# Levels: every Monde near -30 dB RMS over a minute (NO STARS and LOOPS OF ASH, sparse by design, near -36), measured with Rhizome's
# voices by tests/next_features.cpp; the tracks' levels above are the balance inside a Monde, these the Monde's own.
GAIN={'LAST LIGHT':2.22,'AMBER HOUR':1.77,'LONG SHADOWS':2.85,'SLOW EMBERS':1.42,'NIGHT SWALLOWS':1.57,'BLACK ORCHARD':1.51,'SLEEPLESS TIDE':2.05,'NO STARS':0.88,'FIRST BIRD':2.98,'PALE COMET':2.99,'CHOKED DAWN':1.01,'DAY BREAKS':1.13,'COLD IN THE DARK':1.51,'KEPLER NIGHT':1.84,'TRAPPIST DUST':0.99,'RAGA OF ASH':2.12,'SLOW TIDES':7.84,'LOOPS OF ASH':1.49}

# The HALL as a voice of its own: the melodic tracks' sends, raised from the values above (where it sat 15 to 20 dB under
# the direct sound and went unheard) to about 10 dB under it; the bass keeps its small send, for a clear low end.
HALL=2.2

# On the FM-1 a sounding voice costs 6 to 13 % of the processor and a pad's tail keeps it awake for long (measured
# 4 November 2026): a Monde plays four voices at most. Those written with five lose their fifth (track 5).
VOICES_MAX=4

def rhizome(tones,fields):
 """Catalog entries: the FDDS writer's v10 part, plus 'v11', Rhizome's part."""
 out=fdds(tones,fields,0,mondes=MONDES+[BLANK],seed=0x52485A00)
 for b,m in zip(out,MONDES+[BLANK]):
  for i,tr in enumerate(b['tracks']):
   tr['prob']=1000 # the seed is locked: a gate that misses would miss on every turn
   tr['level']=min(1000,round(tr['level']*GAIN.get(m['name'],1)))
   if i and m is not BLANK:b['ext']['tracks'][i]['reverb_send']=min(1000,round(b['ext']['tracks'][i]['reverb_send']*HALL)) # the bass stays dry
   if i in m['orbits']:tr['hits']=0 # a body plays its gates, not a rhythm
  playing=[i for i in range(6) if i in m['orbits'] and m['orbits'][i][0]>0]
  for i in playing[VOICES_MAX:]:b['tracks'][i]['mute']=1;b['tracks'][i]['level']=0 # beyond four voices: silent
  orbits={i:o for i,o in m['orbits'].items() if i not in playing[VOICES_MAX:]}
  prec,pert,comet,event,amount,conj=m['sky'][:6];system=SYSTEM[m['sky'][6]] if len(m['sky'])>6 else 0
  b['v11']=dict(orbit=[orbits.get(i,body(0,16)) for i in range(6)],sky=[prec,pert,COMET[comet],EVENT[event],amount,CONJ[conj],system],
   tape=list(m['tape']),hall=list(m['hall']),beads=list(m['beads'][:3])+[MEDIA[m['beads'][3]]])
  b['v10']=False;b['v11ok']=True
  if m is BLANK:b['seed']=0x52485A00+12*7919 # EMPTY SKY keeps the seed it had as the 13th, before the DEEP Mondes
 return out

def c_patch11(b,fields):
 """The C initializer of a complete v11 md_patch: the v10 part, then the orbits and Rhizome's effects."""
 j=lambda xs:','.join(map(str,xs))
 v=b['v11'];head=c_patch(b,fields).split(',',2);head[1]='12';head=','.join(head) # magic, version, seed
 return head+',{'+','.join('{'+j(o)+'}' for o in v['orbit'])+'},'+j(v['sky']+v['tape']+v['hall']+v['beads'])
