# SPDX-License-Identifier: GPL-3.0-only
"""Rhizome's sound library: 16 sounds for each of its eight voices (all Plaits engines, none of Monades'), read by
tools/generate_presets.py. On the FM-1, K1 TONE is Plaits' TIMBRE, K2 HARMONICS its HARMONICS and K3 SHAPE its MORPH;
values are 0..1000. Long, soft envelopes: Rhizome plays space and time, Monades the notes and the beat."""

MODELS=['STRING MACHINE','WAVETABLE','WAVE TERRAIN','SIX-OP FM','SWARM','PHASE DISTORT','CHORDS','FORMANT']
# Plaits' chords (STRING MACHINE and CHORDS, K2): octave, fifth, sus4, minor, m7, m9, m11, 6/9, maj9, maj7, major.
CHORD={'OCT':0,'5':1,'SUS4':2,'MIN':3,'M7':4,'M9':5,'M11':6,'69':7,'MAJ9':8,'MAJ7':9,'MAJ':10}
def chord(name):return round((CHORD[name]+.5)/11*1000)
# SIX-OP's sounds: the 96 DX7 patches of Plaits' three banks (K2 picks one: index k at (k+.5)/96, the knob read x1.02).
DX={'WINEGLASS':23,'FAIRLIGHT':25,'E.PIANO 1':32,'WINTRHODES':34,'VIBE 1':50,'TUB BELLS':54,'PIPES 1':69,'SOUNDTRACK':73,
 'ICE PAD 2':74,'M1 PADS':75,'CARLOS 2':76,'PLANETS':78,'CIRRUS':79,'TEXTURES 6':82,'ETHERIAL5A':83,'AIRY':84,'VANGELIS 1':86,
 'STRINGS C':87,'SOLID BASS':0,'RESONANCES':9,'SPIRAL':18}
def dx(name):return round((DX[name]+.5)/(96*1.02)*1000)

# name, K1 tone, K2 harmonics, K3 shape, attack, decay, cutoff, filter (0 off, 1 LP 12 dB), breathe
LIBRARY={
 'STRING MACHINE':[
  ('Solina Dusk',450,chord('MIN'),300,600,900,760,1,220),('Ensemble Haze',650,chord('M9'),550,700,920,720,1,260),
  ('Cold Strings',300,chord('5'),200,550,880,820,1,180),('Octave Veil',500,chord('OCT'),700,750,950,700,1,240),
  ('Night Ensemble',600,chord('M7'),450,650,930,680,1,300),('Pale Orchestra',400,chord('MAJ9'),350,600,900,780,1,200),
  ('Thin Strings',250,chord('SUS4'),150,500,860,860,1,160),('Warm Ensemble',700,chord('69'),600,700,940,700,1,280),
  ('Distant Strings',550,chord('M11'),800,850,960,640,1,320),('Organ Strings',350,chord('MAJ7'),900,400,900,760,1,180),
  ('Glass Ensemble',800,chord('MAJ'),250,500,880,860,1,220),('Low Strings',400,chord('OCT'),400,650,920,560,1,200),
  ('Wide Ensemble',900,chord('M7'),500,700,940,720,1,260),('Frozen Strings',200,chord('5'),650,800,960,620,1,340),
  ('Hollow Choir',600,chord('SUS4'),850,750,930,680,1,240),('Last Ensemble',750,chord('MIN'),600,900,980,600,1,360)],
 'WAVETABLE':[
  ('Soft Table',200,60,150,300,820,760,1,160),('Glass Rows',600,120,500,250,780,860,1,180),('Hollow Table',350,320,300,350,840,720,1,200),
  ('Slow Scan',500,380,650,500,900,700,1,300),('Breath Table',300,600,400,450,860,680,1,260),('Vowel Table',650,560,700,300,820,760,1,220),
  ('Metal Rows',800,820,300,200,760,840,1,160),('Bell Table',700,900,600,100,700,880,1,140),('Dust Table',450,700,850,400,880,660,1,280),
  ('Night Table',250,260,550,550,920,640,1,300),('Bright Scan',850,180,800,250,800,880,1,200),('Low Table',300,90,250,350,860,560,1,180),
  ('Ghost Rows',550,460,900,600,940,620,1,340),('Pulse Table',700,780,150,150,760,800,1,160),('Choir Table',400,520,500,500,900,700,1,260),
  ('Faded Scan',600,960,750,700,950,600,1,320)],
 'WAVE TERRAIN':[
  ('Low Terrain',250,60,300,300,860,560,1,200),('Dune Path',450,180,450,400,880,700,1,240),('Ridge Walk',650,300,600,350,840,760,1,220),
  ('Valley Hum',300,420,250,500,900,620,1,260),('Moss Field',500,540,500,450,880,680,1,280),('Stone Circle',700,660,350,300,820,800,1,180),
  ('Salt Flats',400,780,700,500,900,720,1,300),('Deep Crater',200,900,550,600,940,520,1,320),('Night Plain',350,120,800,650,950,600,1,340),
  ('Wind Ridge',800,240,650,400,860,860,1,240),('Slow Orbit',550,360,900,700,960,660,1,360),('Glacier',600,480,150,550,920,780,1,200),
  ('Ash Field',450,600,850,500,900,640,1,300),('Iron Hill',750,720,450,250,800,840,1,180),('Mist Basin',300,840,700,750,960,580,1,340),
  ('Far Terrain',650,960,600,800,970,620,1,380)],
 'SIX-OP FM':[
  ('DX Ice Pad',500,dx('ICE PAD 2'),500,400,940,880,1,200),('DX M1 Pads',500,dx('M1 PADS'),500,450,940,860,1,220),
  ('DX Soundtrack',500,dx('SOUNDTRACK'),500,500,950,860,1,240),('DX Planets',500,dx('PLANETS'),500,400,950,880,1,200),
  ('DX Cirrus',500,dx('CIRRUS'),500,500,950,860,1,260),('DX Textures',500,dx('TEXTURES 6'),500,450,940,860,1,220),
  ('DX Etherial',500,dx('ETHERIAL5A'),500,550,960,860,1,260),('DX Airy',500,dx('AIRY'),500,500,950,880,1,240),
  ('DX Vangelis',500,dx('VANGELIS 1'),500,400,950,860,1,220),('DX Strings',500,dx('STRINGS C'),500,450,940,840,1,200),
  ('DX Wineglass',500,dx('WINEGLASS'),500,0,900,900,1,120),('DX Fairlight',500,dx('FAIRLIGHT'),500,250,920,860,1,180),
  ('DX E.Piano',450,dx('E.PIANO 1'),500,0,850,840,1,100),('DX Winter Rhodes',450,dx('WINTRHODES'),500,0,860,840,1,100),
  ('DX Tub Bells',500,dx('TUB BELLS'),500,0,900,900,1,100),('DX Vibe',450,dx('VIBE 1'),500,0,820,860,1,100)],
 'SWARM':[
  ('Soft Swarm',300,150,500,600,920,620,1,240),('Bee Cloud',600,300,400,500,900,720,1,220),('Drone Hive',400,80,700,700,950,600,1,300),
  ('Wide Swarm',500,500,600,600,930,680,1,260),('Night Swarm',250,250,800,750,960,560,1,340),('Bright Swarm',800,350,350,400,880,820,1,200),
  ('Thick Cloud',700,650,750,650,950,640,1,300),('Thin Swarm',200,200,200,500,900,700,1,200),('Wild Swarm',650,850,500,500,920,700,1,260),
  ('Low Hive',350,100,600,650,940,520,1,280),('Glass Swarm',900,420,300,300,860,880,1,180),('Slow Cloud',450,300,950,850,970,600,1,360),
  ('Dust Swarm',550,700,650,600,940,620,1,300),('Choir Swarm',400,400,550,700,950,660,1,320),('Storm Hive',750,950,800,500,930,700,1,280),
  ('Far Swarm',300,550,900,900,980,560,1,380)],
 'PHASE DISTORT':[
  ('Soft Phase',150,300,500,300,860,760,1,180),('Warm Reed',350,250,300,250,820,740,1,160),('Glass Phase',600,700,500,150,800,860,1,140),
  ('Velvet CZ',250,450,700,450,900,700,1,220),('Hollow Phase',400,550,200,400,880,700,1,200),('Bright CZ',750,350,600,150,780,880,1,140),
  ('Low Phase',300,150,400,300,860,560,1,180),('Night Reed',200,600,800,500,920,640,1,260),('Bell Phase',650,850,350,50,760,880,1,120),
  ('Pale Lead',450,400,500,200,820,820,1,160),('Wide Phase',550,650,650,500,920,720,1,240),('Dark CZ',300,800,250,450,900,600,1,220),
  ('Breath Phase',350,500,900,600,940,660,1,300),('Iron Reed',800,950,450,200,800,840,1,160),('Cloud Phase',250,350,950,700,960,640,1,320),
  ('Last Phase',500,750,750,650,950,680,1,280)],
 'CHORDS':[
  ('Organ Minor',300,chord('MIN'),150,400,900,760,1,180),('Organ Fifths',300,chord('5'),100,350,880,780,1,160),
  ('String Minor 7',400,chord('M7'),450,550,930,720,1,240),('String Minor 9',450,chord('M9'),500,600,940,700,1,260),
  ('Sus Choir',350,chord('SUS4'),600,600,940,700,1,260),('Open Octaves',250,chord('OCT'),300,450,900,700,1,200),
  ('Major Nine',500,chord('MAJ9'),550,550,930,740,1,220),('Six Nine',450,chord('69'),500,550,930,740,1,220),
  ('Eleven Mist',500,chord('M11'),700,700,950,680,1,300),('Major Seven',400,chord('MAJ7'),400,500,920,760,1,200),
  ('Wave Minor',600,chord('MIN'),850,600,940,700,1,260),('Wave Fifths',650,chord('5'),900,650,950,680,1,280),
  ('Low Minor',300,chord('MIN'),350,500,920,560,1,200),('High Major',750,chord('MAJ'),450,450,900,820,1,180),
  ('Dark Eleven',350,chord('M11'),250,650,950,620,1,300),('Last Chord',550,chord('M9'),750,800,970,640,1,340)],
 'FORMANT':[
  ('Breath Vowel',300,250,600,500,900,700,1,260),('Night Voice',250,400,500,600,930,660,1,300),('Choir Ah',400,300,400,500,920,720,1,240),
  ('Choir Oo',200,200,350,550,930,640,1,260),('Hollow Throat',350,550,700,450,900,680,1,280),('Whisper',600,700,900,400,880,760,1,320),
  ('Low Hum',150,150,300,500,920,560,1,220),('Glass Vowel',700,500,300,300,860,860,1,180),('Soft Formant',400,650,550,450,900,700,1,240),
  ('Cold Breath',550,850,800,500,920,720,1,300),('Distant Voice',300,350,650,700,950,620,1,320),('Bright Ah',750,250,450,300,860,840,1,200),
  ('Throat Drone',250,750,350,650,950,580,1,300),('Wind Vowel',650,950,850,600,940,700,1,340),('Monk',200,450,250,600,940,560,1,260),
  ('Last Voice',450,600,750,800,970,640,1,360)],
}

def tones():
 """The library as tone dicts, in the bank's order (16 per voice, voices in MODELS order)."""
 out=[]
 for model,name in enumerate(MODELS):
  rows=LIBRARY[name];assert len(rows)==16,name
  for (n,tone,harm,shape,attack,decay,cutoff,filt,breathe) in rows:
   out.append(dict(id=len(out),name=n,model=model,tone=tone,harm=harm,decay=decay,attack=attack,shape=shape,cutoff=cutoff,resonance=0,filter=filt,breathe=breathe))
 names=[t['name'] for t in out];assert len(set(names))==len(names),'two sounds share a name'
 return out
