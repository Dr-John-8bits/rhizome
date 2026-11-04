#!/usr/bin/env python3
"""Authoring source for the factory atlas. Deterministic, no runtime random bank.
Generated C tables live in flash; JSON supplies the browser's matching labels.
Rhizome: the 224 tones of Monades, 32 of its own (SIX-OP FM and STRING MACHINE, tools/rhizome_tones.py), and its own
Mondes (tools/rhizome_mondes.py) in place of Monades' 91.
"""
from pathlib import Path
import json
R=Path(__file__).resolve().parents[1]
fields='model tone harm decay level send hits length rotate prob note pan mute speed attack ratchet accent shape cutoff resonance filter breathe'.split()
tone_fields='model tone harm decay attack shape cutoff resonance filter breathe'.split()
names=[
'Felt Foot|Deep Pillow|Round Sub|Moss Kick|Soft Knock|Dub Heart|Clay Drum|Warm Pulse|Low Pebble|Thud Room|Rubber Step|Satin Kick|Stone Floor|Dust Punch|Long Earth|Tight Seed',
'Brush Skin|Paper Snare|Muted Rim|Sand Brush|Velvet Snap|Dry Twig|Soft Clap|Dust Snare|Thin Shell|Wood Snare|Loose Skin|Grain Snap|Rain Stick|Low Rattle|Warm Wire|Short Ember',
'Silk Hat|Mist Tick|Dry Leaf|Closed Dust|Shaker Sand|Open Air|Fine Rain|Soft Metal|Leaf Shake|Glass Dust|Tiny Bell|Loose Seeds|Steam Hat|Warm Ride|Grain Rust|Short Hiss',
'Pure Water|Sine Pearl|Wool Bass|Glass Drop|Soft Tine|Rubber Bell|Warm Reed|Amber Keys|Low Orbit|Velvet FM|Copper Bell|Hollow Chime|Dream Marimba|Deep Bloom|Moon Tone|Slow Glass',
'Round Reed|Wool Pad|Satin Bass|Soft Saw|Warm Strings|Hollow Flute|Dust Organ|Muted Pluck|Slow Brass|Cedar Pad|Moss Pulse|Silk Lead|Low Current|Warm Fifth|Cloud Reed|Felt Keys',
'Wood Bowl|Warm Wire|Clay Bell|Moss String|Glass Pool|Soft Steel|Hollow Log|Deep Vessel|Copper Rain|Silk String|Small Stone|Low Gong|Paper Plate|Ice Bell|Amber Chime|Felt Mallet',
'Sine Lantern|Warm Organ|Odd Velvet|Soft Spectrum|Hollow Pipe|Amber Pad|Glass Choir|Low Partial|Silk Harmonic|Felt Organ|Slow Aurora|Moss Chord|High Mist|Bronze Bloom|Deep Spectrum|Pure Harmonic',
'Minor Cloud|Major Dawn|Suspended Air|Open Fifth|Velvet Choir|Sunlit Voices|Floating Sus|Deep Fifth|Minor Moss|Major Silk|Suspended Dew|Wide Horizon|Night Voices|Warm Meadow|Slow Halo|Low Shelter']
# Individual contour tables, rather than 128 copies with arbitrary names.
tones=[]
t=[55,90,140,205,280,340,410,480,100,170,260,350,460,560,70,220]
h=[60,150,280,420,560,700,840,950,220,370,510,640,780,900,100,320]
d=[250,700,420,180,550,820,330,620,920,480,200,760,380,600,1000,120]
a=[0,0,0,0,100,0,160,0,450,300,0,0,80,650,800,350]
for model,row in enumerate(names):
 for j,name in enumerate(row.split('|')):
  attack=a[j] if model in [3,4,6,7] else 0
  shape=([500,500,490,510,485,520,500,505,495,500,530,470,510,500,500,515][j] if model==3 else 750 if model==6 else 80+j*34 if model==4 else 90+j*31)
  tone=t[j] if model!=7 else 100+j*28
  harm=h[j] if model!=7 else [0,300,600,900][j%4]
  cutoff=([660,620,570,720,690,740,680,710,550,640,760,630,700,590,610,730][j])
  if model==2:cutoff=min(950,cutoff+170)
  if model==0:cutoff-=50
  tones.append(dict(id=len(tones),name=name,model=model,tone=tone,harm=harm,decay=d[j],attack=attack,shape=shape,cutoff=cutoff,resonance=35+(j%5)*45,filter=3 if j in [3,7,12,15] else 2 if j in [2,8,9,13] else 1,breathe=(180+(j%4)*80) if attack else (70 if j%3==0 else 0)))
# Keep pitched presets on musical oscillator-ratio plateaus. VA's original
# HARMONICS control spans continuously between intervals; arbitrary values
# can detune the second oscillator by several non-scale semitones.
for x in tones:
 j=x['id']%16;m=x['model']
 if m==3:x.update(harm=[234,234,477,649,477,750,234,649,477,234,750,649,477,234,750,649][j],shape=500,tone=min(x['tone'],380))
 if m==4:x.update(harm=[500,500,500,622,744,500,622,500,744,500,622,500,500,622,744,500][j])
 if m==6:x.update(tone=[0,25,45,60,20,35,75,0,50,15,40,65,85,55,5,0][j],shape=800)
 if m==5:x.update(decay=min(x['decay'],650))
 x['resonance']=0 if m>=3 else min(x['resonance'],70)
 if m==5:x['harm']=[260,280,310,270,300,340,265,285,330,275,260,320,295,350,315,270][j]
 if m in [3,4,6,7]:x['cutoff']=min(930,x['cutoff']+130)
 if m==7:x['shape']=min(x['shape'],220)
# Additional 1.1 palettes (Monades' tone IDs are kept).
bass_names=['Velvet Foundation','Deep Root','Round Step','Soft Rubber','Warm Undertow','Dub Anchor','Low Lantern','Sub Bloom','Wooden Pulse','Gentle Current','Round Motion','Night Floor','Felt Octave','Amber Bass','Slow Earth','Clear Foundation']
strike_names=['Felt Mallet','Moss Marimba','Wooden Bell','Clay Bowl','Amber Bar','Soft Glass','Muted Metal','Low Vessel','Gentle Pluck','Reed Pluck','Warm Tine','Quiet Wire','Sand Seeds','Pebble Rain','Dust Bells','Soft Chimes']
for model,names in [(8,bass_names),(9,strike_names)]:
 for j,name in enumerate(names):
  tones.append(dict(id=len(tones),name=name,model=model,tone=(80+j*25 if model==8 else 100+(j%8)*45),harm=(80+(j%5)*110 if model==8 else 80+(j%8)*95),decay=(80+(j%6)*55 if model==8 else 150+(j%6)*70),attack=0,shape=(100 if model==8 else 100 if j<8 else 500 if j<12 else 800),cutoff=(400+(j%4)*80 if model==8 else 450+(j%5)*60),resonance=0,filter=1,breathe=0))
# 1.1 Mellow collection: new IDs, existing 1.0 factory data unchanged.
for group,model in [('Mellow',10),('Velvet',8),('Hearth',4)]:
 names=['Candle','Linen','Dusk','Willow','Honey','Ember','Lagoon','Moss','Cloud','Dune','Cedar','Haze','Amber','Shelter','Wool','Moon']
 for j,name in enumerate(names):
  tones.append(dict(id=len(tones),name=group+' '+name,model=model,tone=60+(j%4)*65,harm=500 if model==4 else 80+(j%4)*70,decay=450+(j%4)*150,attack=[0,220,450,650][j%4],shape=120+(j%4)*180,cutoff=450+(j%4)*65,resonance=0,filter=1,breathe=100+(j%3)*100))
# Satin Morph: original rounded wavetable family; preserve every earlier ID.
for j,name in enumerate(['Soft Sine','Wool Triangle','Hollow Reed','Rounded Saw','Dusky Organ','Linen Strings','Cedar Bass','Honey Pulse','Felt Keys','Slow Amber','Velour Fifth','Quiet Brass','Warm Current','Paper Flute','Deep Satin','Morning Ribbon']):
 tones.append(dict(id=len(tones),name=name,model=11,tone=700+(j%3)*100,harm=[0,333,665,1000][j%4],decay=500+(j%4)*130,attack=[0,250,450,600][j%4],shape=200+(j%4)*160,cutoff=650+(j%3)*65,resonance=0,filter=1,breathe=120))
# Rhizome: its two own voices, 16 sounds each (tools/rhizome_tones.py), after Monades' 224.
from rhizome_tones import tones as rhizome_tones
tones+=rhizome_tones(len(tones))
# Rhizome: its own Mondes, complete v13 patches (tools/rhizome_mondes.py).
from rhizome_mondes import rhizome,c_patch11,FAMILIES,SHORT
biomes=rhizome(tones,fields)
for i,b in enumerate(biomes):b['id']=i
catalog=dict(version='rhizome-1',families=FAMILIES,family_short=SHORT,tone_fields=tone_fields,tones=tones,biomes=biomes)
(R/'presets/catalog.json').write_text(json.dumps(catalog,indent=2,ensure_ascii=False)+'\n')
lines=['/* Generated by tools/generate_presets.py; MIT. Flash-resident factory atlas. */','struct TonePreset { const char *name; int32_t '+','.join(tone_fields)+'; };','static const TonePreset tone_bank[MD_TONE_PRESETS]={']
for t in tones:lines.append('{'+json.dumps(t['name'])+','+','.join(str(t[k]) for k in tone_fields)+'},')
lines+=['};','struct BiomePreset { const char *name; unsigned family; md_patch patch; };','static const char *biome_families[]={'+','.join(json.dumps(x) for x in SHORT)+'};','static const BiomePreset biome_bank[MD_BIOMES]={']
for b in biomes:lines.append('{'+json.dumps(b['name'])+','+str(b['family'])+',{'+c_patch11(b,fields)+'}},')
lines+=['};'];(R/'app/dsp/preset_bank.inc').write_text('\n'.join(lines)+'\n')
print(f'{len(biomes)} Mondes / {len(tones)} tone presets generated')
