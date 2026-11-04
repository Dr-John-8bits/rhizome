# SPDX-License-Identifier: GPL-3.0-only
"""Rhizome's own sounds: 16 for each of its two own voices, SIX-OP FM and STRING MACHINE (Plaits engines), after
Monades' 224, read by tools/generate_presets.py. On the FM-1, K1 is Plaits' TIMBRE, K2 its HARMONICS (the DX7 sound, the
chord) and K3 its MORPH; values are 0..1000. Long, soft envelopes: Rhizome plays space and time."""

MODELS={'SIX-OP FM':12,'STRING MACHINE':13} # app/dsp/dreams.h: MD_SIXOP, MD_STRINGS
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
 'SIX-OP FM':[
  ('DX Ice Pad',500,dx('ICE PAD 2'),500,400,940,880,1,200),('DX M1 Pads',500,dx('M1 PADS'),500,450,940,860,1,220),
  ('DX Soundtrack',500,dx('SOUNDTRACK'),500,500,950,860,1,240),('DX Planets',500,dx('PLANETS'),500,400,950,880,1,200),
  ('DX Cirrus',500,dx('CIRRUS'),500,500,950,860,1,260),('DX Textures',500,dx('TEXTURES 6'),500,450,940,860,1,220),
  ('DX Etherial',500,dx('ETHERIAL5A'),500,550,960,860,1,260),('DX Airy',500,dx('AIRY'),500,500,950,880,1,240),
  ('DX Vangelis',500,dx('VANGELIS 1'),500,400,950,860,1,220),('DX Strings',500,dx('STRINGS C'),500,450,940,840,1,200),
  ('DX Wineglass',500,dx('WINEGLASS'),500,0,900,900,1,120),('DX Fairlight',500,dx('FAIRLIGHT'),500,250,920,860,1,180),
  ('DX E.Piano',450,dx('E.PIANO 1'),500,0,850,840,1,100),('DX Winter Rhodes',450,dx('WINTRHODES'),500,0,860,840,1,100),
  ('DX Tub Bells',500,dx('TUB BELLS'),500,0,900,900,1,100),('DX Vibe',450,dx('VIBE 1'),500,0,820,860,1,100)],
}

def tones(first):
 """The 32 sounds as tone dicts, numbered from first (SIX-OP FM's 16, then STRING MACHINE's)."""
 out=[]
 for name,model in MODELS.items():
  rows=LIBRARY[name];assert len(rows)==16,name
  for (n,tone,harm,shape,attack,decay,cutoff,filt,breathe) in rows:
   out.append(dict(id=first+len(out),name=n,model=model,tone=tone,harm=harm,decay=decay,attack=attack,shape=shape,cutoff=cutoff,resonance=0,filter=filt,breathe=breathe))
 return out
