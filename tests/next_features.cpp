#include "dsp/dreams.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <vector>
static float audio[256];
static double run(int blocks){double e=0;for(int b=0;b<blocks;b++){md_render(audio,128);for(float x:audio){assert(std::isfinite(x)&&fabsf(x)<1);e+=x*x;}}return e;}
static void cmd(int c,int t,int v){assert(md_command(c,t,v));run(1);}
static md_patch blank(int model){md_init();cmd(MD_SCENE,0,MD_BLANK_BIOME);md_patch p=*md_state();p.track[0].mute=0;p.track[0].model=model;p.track[0].note=36;p.track[0].level=650;p.track[0].decay=200;p.track[0].attack=0;p.track[0].hits=16;p.track[0].length=16;p.track[0].prob=1000;p.macro[0]=500;p.macro[1]=0;p.macro[2]=500;p.macro[3]=0;return p;}
static uint64_t hash(const md_patch &p){md_init();assert(md_load(&p));cmd(MD_PLAY,0,1);uint64_t h=1469598103934665603ull;for(int b=0;b<2200;b++){md_render(audio,128);for(float x:audio){uint32_t u;memcpy(&u,&x,4);h=(h^u)*1099511628211ull;}}return h;}
int main(){
 static_assert(sizeof(md_patch)==1976,"Browser patch size must match");
 auto p=blank(MD_BASS);assert(md_load(&p));cmd(MD_TRIGGER,0,36);assert(run(300)>1);puts("PASS round bass audible");
 uint8_t legacy[652];memcpy(legacy,&p,652);uint32_t v=4;memcpy(legacy+4,&v,4);assert(md_import(legacy,sizeof legacy));assert(md_state()->version==13&&md_state()->fx_level==0);puts("PASS 1.0 migration retains old fields and disables additions");
 p=*md_state();p.fx_level=1001;assert(!md_load(&p));p=*md_state();p.ext[0].notes[15]=25;assert(!md_load(&p));
 p=blank(MD_BASS);p.ext[0].bass_mode=1;for(int &n:p.ext[0].notes)n=-1;assert(md_load(&p));cmd(MD_PLAY,0,1);assert(run(3000)==0&&md_status()->events[0]==0);puts("PASS phrase rests do not trigger");
 p=blank(MD_BASS);p.ext[0].bass_mode=1;p.ext[0].marbles=1000;p.locked=1;assert(hash(p)==hash(p));auto a=hash(p);p.ext[0].marbles=0;assert(a!=hash(p));puts("PASS Marbles remembered variation deterministic and audible");
 p=blank(MD_VA);p.track[0].filter=1;p.track[0].cutoff=500;a=hash(p);p.ext[0].tides=800;assert(a!=hash(p));puts("PASS Tides changes filtered audio");
 p=blank(MD_BASS);a=hash(p);p.ext[0].warps=800;assert(a!=hash(p));puts("PASS Warps ring blend changes audio");
 p=blank(MD_BASS);p.track[0].decay=0;p.ext[0].delay_send=800;p.ext[0].reverb_send=800;p.fx_level=800;p.delay_fb=800;p.reverb_time=850;md_init();assert(md_load(&p));cmd(MD_TRIGGER,0,36);run(300);assert(run(800)>0.001);cmd(MD_PANIC,0,0);assert(run(1000)==0);puts("PASS shared effect tails / panic exact silence");
 for(int quality=0;quality<=1;quality++)for(int model: {MD_BASS,MD_ELEMENTS,MD_VA,MD_RINGS}){p=blank(model);for(int i=0;i<6;i++){p.track[i]=p.track[0];p.ext[i].bass_mode=1;p.ext[i].delay_send=800;p.ext[i].reverb_send=800;p.ext[i].tides=600;p.ext[i].warps=500;p.ext[i].marbles=800;p.ext[i].glide=400;}p.quality=quality;p.fx_level=750;p.delay_fb=800;p.reverb_time=850;p.tempo=220;md_init();assert(md_load(&p));cmd(MD_PLAY,0,1);for(int j=0;j<90;j++){cmd(MD_DELAY_TIME,0,j%2?40:350);cmd(MD_GRAIN_SIZE,0,(j*73)%1001);run(230);}assert(!md_status()->nonfinite);printf("PASS 60-second feature stress model %d\n",model);}
 p=blank(MD_MELLOW);assert(md_load(&p));md_midi(0x90,48,127,0);assert(run(100)>1);md_midi(0x80,48,0,0);run(100);assert(run(300)<.0001);puts("PASS MIDI note off releases Mellow");
 for(int note: {24,48,72,96}){p=blank(MD_SATIN);p.track[0].note=note;p.track[0].tone=950;p.track[0].harm=1000;p.track[0].attack=0;assert(md_load(&p));md_midi(0x90,note,110,0);assert(run(100)>0.01);for(int h=0;h<=1000;h+=10){cmd(MD_HARM,0,h);run(3);}md_midi(0x80,note,0,0);run(100);assert(run(300)<.0001);}puts("PASS Satin morph / high and low notes / MIDI release");
 p=blank(MD_MELLOW);assert(md_load(&p));md_midi(0x90,48,100,0);run(10);md_midi(0xb0,64,127,0);md_midi(0x90,48,0,0);assert(run(100)>1);md_midi(0xb0,64,0,0);run(100);assert(run(300)<.0001);puts("PASS sustain and velocity-zero note off");
 p=blank(MD_MELLOW);assert(md_load(&p));md_midi(0x90,48,40,0);double quiet=run(100);md_init();assert(md_load(&p));md_midi(0x90,48,120,0);double loud=run(100);assert(loud>quiet*7&&loud<quiet*11);md_midi(0xb0,74,32,0);assert(md_state()->track[0].cutoff==32*1000/127);puts("PASS velocity energy and MIDI CC routing");
 p=blank(MD_BASS);p.track[0].hits=0;p.ext[0].bass_mode=2;p.track[0].length=4;for(int &v:p.ext[0].notes)v=-1;p.ext[0].notes[0]=7;assert(md_load(&p));cmd(MD_PLAY,0,1);run(1000);assert(md_status()->events[0]>0&&md_status()->last_note[0]==43);puts("PASS independent Step sequence with Euclidean hits zero");
 md_init();cmd(MD_CLOCK_SOURCE,0,1);md_midi(0xfa,0,0,2);assert(!md_playing());md_midi(0xfa,0,0,0);assert(md_playing());md_midi(0xf8,0,0,0);assert(md_status()->step==0);for(int k=0;k<6;k++)md_midi(0xf8,0,0,0);assert(md_status()->step==1);run(800);assert(!md_playing());puts("PASS selected external clock and loss timeout");
 p=blank(MD_ELEMENTS);for(int i=0;i<6;i++)p.track[i]=p.track[0];assert(md_load(&p));cmd(MD_PLAY,0,1);run(10);assert(md_status()->element_modes==2);for(int i=0;i<6;i++)assert(md_status()->events[i]>0);puts("PASS shared Elements budget keeps six tracks active");
 md_init();md_midi(0xb0,0,1,0);md_midi(0xc0,32,0,0);assert(md_state()->tone_preset[0]==160);md_midi(0xc0,33,0,0);assert(md_state()->tone_preset[0]==161);md_midi(0xbf,16,100,0);assert(md_state()->macro[0]==100*1000/127);puts("PASS program bank retained and global channel macros");
 // Rapid plucks must contain the main impulse, not only the negative primer.
 for(int shape: {334,500,666}){
  p=blank(MD_ELEMENTS);p.track[0].shape=shape;p.track[0].harm=280;
  p.track[0].tone=400;p.track[0].decay=700;p.track[0].ratchet=4;p.tempo=220;
  p.track[0].filter=0;p.macro[3]=0;p.fx_level=0;md_init();assert(md_load(&p));
  cmd(MD_PLAY,0,1);double energy=run(2000);
  printf("Elements rapid pluck shape %d energy %.9f\n",shape,energy);fflush(stdout);
  // Missing main pulses measured 3.46 here; intact plucks exceed 1300.
  assert(energy>100);assert(md_status()->nonfinite==0);
 }
 puts("PASS rapid Plectrum rolls remain audible across the complete shape region");
 {
  // Monades MIDI out: zones (with an overlap), per-track mode, clock, balanced note-offs, no echo of MIDI input.
  uint8_t m[3];int on[16],off[16],clocks,starts,stops;
  auto reset=[&]{memset(on,0,sizeof on);memset(off,0,sizeof off);clocks=starts=stops=0;};
  auto drain=[&]{while(md_midi_take(m)){int st=m[0];if(st==0xf8)clocks++;else if(st==0xfa)starts++;else if(st==0xfc)stops++;else if((st&0xf0)==0x90)on[st&15]++;else if((st&0xf0)==0x80)off[st&15]++;}};
  auto play=[&](const md_midi_routing &r,int blocks){md_patch q=blank(MD_BASS);q.track[0].prob=1000;q.tempo=120;assert(md_load(&q));md_set_midi_routing(&r);drain();reset();
   cmd(MD_PLAY,0,1);for(int b=0;b<blocks;b++){md_render(audio,128);drain();}cmd(MD_PLAY,0,0);drain();};
  md_midi_routing r={};r.mode=2;r.clock=1;r.zone[0].lo=24;r.zone[0].hi=47;r.zone[0].ch=1;r.zone[1].lo=36;r.zone[1].hi=60;r.zone[1].ch=2;r.zone[2].lo=72;r.zone[2].hi=96;r.zone[2].ch=3;
  play(r,689);
  assert(starts==1&&stops==1);assert(clocks>=95&&clocks<=98);
  assert(on[0]>=15&&on[0]==on[1]&&on[2]==0);for(int c=0;c<16;c++)assert(on[c]==off[c]);
  puts("PASS MIDI out zones: overlap sends on both channels, clock 24 ppqn, start/stop, every note released");
  md_midi_routing t1={};t1.mode=1;play(t1,689);
  assert(on[0]>=15&&clocks==0&&starts==0);for(int c=1;c<16;c++)assert(on[c]==0);for(int c=0;c<16;c++)assert(on[c]==off[c]);
  puts("PASS MIDI out per track: track 1 on channel 1, no clock when disabled");
  md_midi_routing none={};play(none,300);for(int c=0;c<16;c++)assert(on[c]==0);assert(clocks==0);
  md_set_midi_routing(&t1);md_midi(0x90,60,100,0);run(10);md_midi(0x80,60,0,0);run(10);drain();for(int c=0;c<16;c++)assert(on[c]==0);
  puts("PASS MIDI out off sends nothing; notes received over MIDI are never echoed");
 }
 {
  // Monades scales: 1.1 indices unchanged, raga ascent / descent, custom scale, browsing order covers every scale.
  const unsigned legacy[8]={0xfff,0x5ad,0xab5,0x6ad,0x4a9,0x295,0x555,0x5ab};
  for(int i=0;i<8;i++)assert(md_scale_mask(i)==legacy[i]&&md_scale_mask_down(i)==legacy[i]);
  int khamaj=-1;for(int i=0;i<md_scale_count();i++)if(!strcmp(md_scale_name(i),"KHAMAJ"))khamaj=i;assert(khamaj>0);
  assert(md_scale_mask(khamaj)==((1u<<0)|(1u<<4)|(1u<<5)|(1u<<7)|(1u<<9)|(1u<<11)));
  assert(md_scale_mask_down(khamaj)==((1u<<0)|(1u<<2)|(1u<<4)|(1u<<5)|(1u<<7)|(1u<<9)|(1u<<10)));
  md_set_custom_scale(0x004);assert(md_scale_mask(md_scale_custom())==0x005);
  assert(md_quantize(63,0,md_scale_custom())==62);
  bool seen[128]={};for(int k=0;k<md_scale_count();k++){int sc=md_scale_at(k);assert(!seen[sc]);seen[sc]=true;assert(md_scale_position(sc)==k);}
  md_patch q=blank(MD_BASS);q.scale=md_scale_count()-1;assert(md_validate(&q));q.scale=md_scale_count();assert(!md_validate(&q));
  printf("PASS %d scales: 1.1 order kept, raga ascent/descent, custom scale, browsing order complete\n",md_scale_count());
 }
 {
  // Monades microtonal: cents quantizing, non-octave periods, MIDI pitch bend in per-track mode only.
  auto find=[](const char *n){for(int i=0;i<md_scale_count();i++)if(!strcmp(md_scale_name(i),n))return i;return -1;};
  int rast=find("RAST"),bp=find("BOHLEN PIERCE"),shruti=find("SHRUTI22");assert(rast>0&&bp>0&&shruti>0);
  assert(fabsf(md_quantize_pitch(63.4f,0,rast)-63.5f)<1e-4f);           /* E half-flat, 350 cents above C */
  assert(fabsf(md_quantize_pitch(70.4f,0,rast)-70.5f)<1e-4f);           /* B half-flat */
  float d[32],per=0;assert(md_scale_cents(bp,d,32,&per)==13&&fabsf(per-1901.955f)<.01f);
  {float lattice=(4*1901.955f+292.6f)*.01f;assert(fabsf(md_quantize_pitch(lattice+.3f,0,bp)-lattice)<.01f);} /* 4 tritaves + 2 steps above the root */
  assert(md_scale_cents(shruti,d,32,&per)==22&&per==1200);
  for(int i=0;i<md_scale_count();i++){float q=md_quantize_pitch(61.3f,2,i);assert(q>=24&&q<=96);}
  int bends=0,on=0;uint8_t m[3];
  md_patch q=blank(MD_VA);q.scale=rast;q.track[0].note=63;q.track[0].prob=1000;assert(md_load(&q));
  md_midi_routing r={};r.mode=1;md_set_midi_routing(&r);while(md_midi_take(m));
  cmd(MD_PLAY,0,1);for(int b=0;b<300;b++){md_render(audio,128);while(md_midi_take(m)){if((m[0]&0xf0)==0xe0)bends++;if((m[0]&0xf0)==0x90)on++;}}cmd(MD_PLAY,0,0);
  assert(on>0&&bends>0);
  r.mode=2;r.zone[0].lo=24;r.zone[0].hi=96;r.zone[0].ch=1;md_set_midi_routing(&r);while(md_midi_take(m));bends=on=0;
  cmd(MD_PLAY,0,1);for(int b=0;b<300;b++){md_render(audio,128);while(md_midi_take(m)){if((m[0]&0xf0)==0xe0)bends++;if((m[0]&0xf0)==0x90)on++;}}cmd(MD_PLAY,0,0);
  assert(on>0&&bends==0);
  puts("PASS microtonal: Rast quarter tones, Bohlen-Pierce tritave, 22 shrutis, pitch bend per track, nearest note in zones");
 }
 {
  // Patch v7: a Mutable Dreams 1.1 patch (v6, 1292 bytes) loads with every Monades addition neutral.
  md_patch q=blank(MD_VA);uint8_t old[1292];memcpy(old,&q,1292);uint32_t v6=6;memcpy(old+4,&v6,4);
  assert(md_import(old,sizeof old));const md_patch *s=md_state();assert(s->version==13&&s->send_hp==0&&s->echo_tone==500&&s->room_tone==500);
  for(int i=0;i<6;i++)for(int k=0;k<2;k++)assert(s->lfo[i][k].depth==0);
  uint32_t v7=7;memcpy(old+4,&v7,4);assert(!md_import(old,sizeof old));
  q=blank(MD_VA);q.lfo[2][1].rate=MD_LFO_RATES;assert(!md_validate(&q));q=blank(MD_VA);q.lfo[0][0].depth=1001;assert(!md_validate(&q));q=blank(MD_VA);q.track[0].filter=MD_FILTER_MODES;assert(!md_validate(&q));
  // Synced LFO: a 1/4 cycle starts again exactly every four steps, from PLAY.
  q=blank(MD_VA);q.tempo=120;q.lfo[0][0]={7,MD_LFO_SINE,MD_TO_CUTOFF,500};assert(md_load(&q));cmd(MD_PLAY,0,1);
  bool seen=false;for(int b=0;b<400&&!seen;b++){md_render(audio,128);if(md_status()->step==8){seen=true;assert(md_status()->lfo_cycle[0][0]==2&&md_status()->lfo_phase[0][0]<.02f);}}
  assert(seen);cmd(MD_PLAY,0,0);
  // Square LFO on the cutoff: the filter really moves; with depth 0 it does not.
  auto swing=[&](int depth){md_patch r=blank(MD_VA);r.track[0].filter=1;r.track[0].cutoff=500;r.track[0].breathe=0;r.ext[0].tides=0;r.lfo[0][0]={10,MD_LFO_SQUARE,MD_TO_CUTOFF,depth};assert(md_load(&r));cmd(MD_PLAY,0,1);
   float lo=1e9f,hi=0;for(int b=0;b<600;b++){md_render(audio,128);if(b>100){float c=md_status()->cutoff_now[0];lo=fminf(lo,c);hi=fmaxf(hi,c);}}cmd(MD_PLAY,0,0);return hi/lo;};
  assert(swing(0)<1.01f&&swing(1000)>8);
  // Sample & hold: one value per cycle, the one the screen draws, and it changes from cycle to cycle.
  q=blank(MD_VA);q.lfo[1][0]={14,MD_LFO_SAMPLE_HOLD,MD_TO_PITCH,300};assert(md_load(&q));cmd(MD_PLAY,0,1);int changes=0;float last=2;
  for(int b=0;b<600;b++){md_render(audio,128);const md_stats *m=md_status();float v=m->lfo_value[1][0];assert(fabsf(v-md_lfo_wave(1,0,m->lfo_cycle[1][0],.7f))<1e-6f);if(v!=last)changes++;last=v;}
  cmd(MD_PLAY,0,0);assert(changes>10);
  // Tremolo never raises the level.
  auto peak=[&](int depth){md_patch r=blank(MD_VA);r.lfo[0][0]={10,MD_LFO_SINE,MD_TO_LEVEL,depth};assert(md_load(&r));cmd(MD_PLAY,0,1);float pk=0;for(int b=0;b<500;b++){md_render(audio,128);pk=fmaxf(pk,md_status()->track_peak[0]);}cmd(MD_PLAY,0,0);return pk;};
  {float flat=peak(0);assert(peak(1000)<=flat*1.02f&&peak(-1000)<=flat*1.02f);}
  puts("PASS LFOs: 1.1 patches migrate, synced phase restarts with PLAY, cutoff moves, sample & hold matches the view, tremolo never boosts");
  // Filter modes: high-pass thins a low note, band and notch stay finite and audible.
  auto energy=[&](int mode,int cutoff){md_patch r=blank(MD_VA);r.track[0].filter=mode;r.track[0].cutoff=cutoff;r.track[0].resonance=0;r.track[0].breathe=0;assert(md_load(&r));cmd(MD_PLAY,0,1);double e=run(400);cmd(MD_PLAY,0,0);return e;};
  {double dry=energy(0,500);assert(energy(4,850)<dry*.5);assert(energy(5,600)>0&&energy(6,600)>0&&energy(6,600)<dry*1.2);}
  // Send high-pass: a kick through the echo loses most of its body.
  auto echo=[&](int hp){md_patch r=blank(MD_KICK);r.ext[0].delay_send=1000;r.fx_level=1000;r.send_hp=hp;assert(md_load(&r));cmd(MD_PLAY,0,1);float pk=0;for(int b=0;b<500;b++){md_render(audio,128);pk=fmaxf(pk,md_status()->fx_peak[1]);}cmd(MD_PLAY,0,0);return pk;};
  {float full=echo(0);assert(full>0&&echo(1000)<full*.5f);}
  assert(md_send_hp_hz(0)==0&&md_send_hp_hz(1)>=20&&md_send_hp_hz(1000)==2000);
  puts("PASS filters: HP / BP / notch, 12 dB send high-pass on the echo and room");
 }
 {
  // HP 24 is steeper than HP 12 at the same cutoff.
  auto energy=[&](int mode){md_patch r=blank(MD_VA);r.track[0].filter=mode;r.track[0].cutoff=800;r.track[0].resonance=0;r.track[0].breathe=0;assert(md_load(&r));cmd(MD_PLAY,0,1);double e=run(400);cmd(MD_PLAY,0,0);return e;};
  assert(energy(7)<energy(4)*.8);
  // Sample & hold on NOTES: generated notes jump around but always stay in the scale (scale 1, root C).
  md_patch q=blank(MD_VA);q.scale=1;q.root=0;q.track[0].note=60;q.lfo[0][0]={10,MD_LFO_SAMPLE_HOLD,MD_TO_NOTES,1000};assert(md_load(&q));cmd(MD_PLAY,0,1);
  unsigned mask=md_scale_mask(1),seen=0;for(int b=0;b<1500;b++){md_render(audio,128);int nt=md_status()->last_note[0];if(nt>0){assert((mask>>(nt%12))&1);seen|=1u<<(nt%12);}}
  cmd(MD_PLAY,0,0);int distinct=0;for(int k=0;k<12;k++)distinct+=(seen>>k)&1;assert(distinct>=4);
  // HITS: a negative square removes hits for half of each bar.
  auto events=[&](int depth){md_patch r=blank(MD_VA);r.lfo[0][1]={3,MD_LFO_SQUARE,MD_TO_HITS,depth};assert(md_load(&r));cmd(MD_PLAY,0,1);run(1500);cmd(MD_PLAY,0,0);return md_status()->events[0];};
  assert(events(-1000)<events(0)*3/4);
  // Echo sync: 1/8 at 120 BPM = 250 ms; 1/4 at 60 BPM (1 s) is halved to fit head 1 (0.95 s at the slowest tape),
  // and fits whole on head 3 (2.86 s).
  q=blank(MD_VA);q.tempo=120;q.echo_sync=6;assert(md_load(&q));assert(fabsf(md_echo_ms()-250)<.01f);
  q.tempo=60;q.echo_sync=9;assert(md_load(&q));assert(fabsf(md_echo_ms()-500)<.01f);q.tape_heads=4;assert(md_load(&q));assert(fabsf(md_echo_ms()-1000)<.01f);q.tape_heads=1;q.echo_sync=0;q.delay_ms=123;assert(md_load(&q));assert(md_echo_ms()==123);
  q.echo_sync=MD_ECHO_SYNCS;assert(!md_validate(&q));
  // Chorus and room diffusion change the sound and stay finite.
  auto out=[&](int chorus,int diffusion){md_patch r=blank(MD_VA);r.chorus=chorus;r.chorus_depth=800;r.room_diffusion=diffusion;r.ext[0].reverb_send=800;r.fx_level=800;return hash(r);};
  uint64_t plain=out(0,0);assert(out(1000,0)!=plain&&out(0,1000)!=plain);
  puts("PASS lot 4: HP24, notes in scale from S&H, hits LFO, echo sync, chorus, room diffusion");
 }
 {
  // Patch v8: a Monades v7 patch (1512 bytes) loads with generators and form off.
  md_patch q=blank(MD_VA);uint8_t v7[1512];memcpy(v7,&q,1512);uint32_t ver=7;memcpy(v7+4,&ver,4);
  assert(md_import(v7,sizeof v7));assert(md_state()->version==13&&md_state()->form==0);for(int i=0;i<6;i++)assert(md_state()->gen[i].source==MD_GEN_OFF&&md_state()->gen[i].loop==8);
  q=blank(MD_VA);q.gen[0].loop=0;assert(!md_validate(&q));q=blank(MD_VA);q.form_bars=4;assert(!md_validate(&q));
  // Generated notes of track 1, one per event.
  auto notes=[&](md_patch r,int count,int *out){assert(md_load(&r));cmd(MD_PLAY,0,1);unsigned last=md_status()->events[0];int n=0;
   for(int b=0;b<20000&&n<count;b++){md_render(audio,64);if(md_status()->events[0]!=last){last=md_status()->events[0];out[n++]=md_status()->last_note[0];}}
   cmd(MD_PLAY,0,0);assert(n==count);};
  int a[48];
  // Déjà-vu 1000: once the 4-slot register is written, the phrase loops exactly.
  q=blank(MD_VA);q.scale=1;q.track[0].note=60;q.gen[0]={MD_GEN_RANDOM,1000,4,12};notes(q,48,a);
  for(int k=4;k<48;k++)assert(a[k]==a[k-4]);
  // Déjà-vu 0: every hit draws anew within the spread (60 +/- 6, then snapped to the scale).
  q.gen[0].dejavu=0;notes(q,48,a);{unsigned seen=0;for(int k=0;k<48;k++){assert(a[k]>=53&&a[k]<=67&&((md_scale_mask(1)>>(a[k]%12))&1));seen|=1u<<(a[k]%12);}int d=0;for(int k=0;k<12;k++)d+=(seen>>k)&1;assert(d>=4);}
  // Middle déjà-vu: a phrase that changes slowly, neither frozen nor random.
  q.gen[0].dejavu=850;notes(q,48,a);{int same=0;for(int k=8;k<48;k++)same+=a[k]==a[k-4];assert(same>20&&same<40);}
  // Markov on the phrase 0 3 7 12 (scale 1 is minor): each note follows one that came after it in the phrase.
  q=blank(MD_VA);q.scale=1;q.track[0].note=60;for(int k=0;k<16;k++)q.ext[0].notes[k]=-1;q.ext[0].notes[0]=0;q.ext[0].notes[1]=3;q.ext[0].notes[2]=7;q.ext[0].notes[3]=12;
  q.gen[0]={MD_GEN_MARKOV,0,8,12};notes(q,40,a);
  for(int k=1;k<40;k++){int x=a[k-1]-60,y=a[k]-60;assert((x==0&&y==3)||(x==3&&y==7)||(x==7&&y==12)||(x==12&&y==0));}
  // Chance frozen by déjà-vu: with Chance 50 % the played steps repeat bar after bar.
  q=blank(MD_VA);q.track[0].prob=500;q.gen[0]={MD_GEN_RANDOM,1000,16,0};assert(md_load(&q));cmd(MD_PLAY,0,1);
  uint32_t bars[4]={};for(int b=0,bar=0;b<20000&&bar<4;b++){md_render(audio,64);if(md_status()->step%16==15){bars[bar]=md_status()->step_played[0];if(md_status()->step/16>=1u+bar)bar++;}}
  cmd(MD_PLAY,0,0);assert(bars[1]==bars[2]&&bars[2]==bars[3]&&bars[1]!=0&&bars[1]!=0xffff);
  puts("PASS generators: v7 migrates, deja-vu loops / mutates / draws anew, Markov follows the phrase, Chance groove freezes");
  // Form: the peak plays everything, calm sections thin out and rest some tracks.
  q=blank(MD_VA);for(int i=1;i<4;i++){q.track[i]=q.track[0];q.track[i].mute=0;}q.form=1000;q.form_bars=0;q.form_drops=3;assert(md_load(&q));
  assert(md_form_mask(0,2)==0&&md_form_mask(0,0)!=0&&md_form_mask(0,0)!=0x0f);
  cmd(MD_PLAY,0,1);unsigned per[4]={},prev=0;for(int b=0;b<20000;b++){md_render(audio,64);unsigned e=0;for(int i=0;i<4;i++)e+=md_status()->events[i];
   if(md_status()->form_cycle>0)break;per[md_status()->form_section]+=e-prev;prev=e;}
  cmd(MD_PLAY,0,0);assert(per[2]>per[0]*3/2&&per[2]>per[3]);
  puts("PASS form: calm / build / peak / release change density and rest tracks");
 }
 {
  // Patch v9: a v8 patch (1620 bytes) loads with no gamaka and no pivot notes.
  md_patch q=blank(MD_VA);uint8_t v8[1620];memcpy(v8,&q,1620);uint32_t ver=8;memcpy(v8+4,&ver,4);
  assert(md_import(v8,sizeof v8));assert(md_state()->version==13&&md_state()->vadi==-1&&md_state()->samvadi==-1);for(int i=0;i<6;i++)assert(md_state()->gamaka[i]==MD_GAMAKA_OFF);
  q=blank(MD_VA);q.vadi=12;assert(!md_validate(&q));q=blank(MD_VA);q.gamaka[3]=MD_GAMAKAS;assert(!md_validate(&q));
  // Kan: the note starts on the scale note above (C major-ish scale 1, from C the next note is D, +2), then lands.
  auto ornament=[&](int style,int samples){md_patch r=blank(MD_VA);r.scale=1;r.root=0;r.track[0].note=60;r.track[0].hits=0;r.gamaka[0]=style;r.gamaka_amount[0]=500;assert(md_load(&r));
   cmd(MD_TRIGGER,0,60);for(int k=0;k<samples/64;k++)md_render(audio,64);return md_status()->gamaka_now[0];};
  assert(fabsf(ornament(MD_GAMAKA_KAN,128)-2)<1e-4f&&ornament(MD_GAMAKA_KAN,8000)==0);
  // Andolan: sways below the note and comes back, never above it.
  {float lo=0,hi=-1;md_patch r=blank(MD_VA);r.scale=1;r.track[0].hits=0;r.gamaka[0]=MD_GAMAKA_ANDOLAN;assert(md_load(&r));cmd(MD_TRIGGER,0,62);
   for(int k=0;k<1400;k++){md_render(audio,64);float o=md_status()->gamaka_now[0];lo=fminf(lo,o);hi=fmaxf(hi,o);}assert(lo< -.5f&&hi<=1e-6f);}
  // Meend: glides from the previous note.
  {md_patch r=blank(MD_VA);r.scale=1;r.track[0].hits=0;r.gamaka[0]=MD_GAMAKA_MEEND;assert(md_load(&r));cmd(MD_TRIGGER,0,60);run(10);cmd(MD_TRIGGER,0,67);
   md_render(audio,64);float o=md_status()->gamaka_now[0];assert(o< -6.5f&&o>=-7.0f);run(200);assert(md_status()->gamaka_now[0]==0);}
  puts("PASS gamakas: kan from the upper scale note, andolan sways below, meend glides from the previous note");
  // Pivot notes: with a vadi on the fifth (G), random notes land on G more often.
  auto count_g=[&](int vadi){md_patch r=blank(MD_VA);r.scale=1;r.root=0;r.track[0].note=60;r.gen[0]={MD_GEN_RANDOM,0,8,12};r.vadi=vadi;assert(md_load(&r));cmd(MD_PLAY,0,1);
   unsigned last=md_status()->events[0];int g=0,n=0;for(int b=0;b<30000&&n<400;b++){md_render(audio,64);if(md_status()->events[0]!=last){last=md_status()->events[0];n++;g+=md_status()->last_note[0]%12==7;}}cmd(MD_PLAY,0,0);return g;};
  assert(count_g(7)>count_g(-1)*5/4);
  // MOTIF source with no variation plays the motif in order, rests skipped.
  {md_patch r=blank(MD_VA);r.scale=1;r.root=0;r.track[0].note=60;for(int k=0;k<16;k++)r.ext[0].notes[k]=-1;r.ext[0].notes[0]=0;r.ext[0].notes[5]=7;r.ext[0].notes[9]=3;
   r.gen[0]={MD_GEN_MOTIF,0,16,0};assert(md_load(&r));cmd(MD_PLAY,0,1);unsigned last=md_status()->events[0];int seq[9],n=0;
   for(int b=0;b<30000&&n<9;b++){md_render(audio,64);if(md_status()->events[0]!=last){last=md_status()->events[0];seq[n++]=md_status()->last_note[0];}}cmd(MD_PLAY,0,0);
   for(int k=1;k<9;k++){int a=seq[k-1],b=seq[k];assert((a==60&&b==67)||(a==67&&b==63)||(a==63&&b==60));}} /* in order, from wherever the capture starts */
  puts("PASS raga: vadi attracts generated notes, MOTIF source walks the motif");
 }
 {
  // Patch v10: a v9 patch (1676 bytes) keeps Motion on every step.
  md_patch q=blank(MD_VA);uint8_t v9[1676];memcpy(v9,&q,1676);uint32_t ver=9;memcpy(v9+4,&ver,4);
  assert(md_import(v9,sizeof v9));assert(md_state()->version==13&&md_state()->motion_every==0);q.motion_every=5;assert(!md_validate(&q));
  // Motion every 4 steps: the variation only changes on the beat.
  q=blank(MD_VA);q.scale=1;q.track[0].note=60;q.macro[1]=1000;q.motion_every=2;assert(md_load(&q));cmd(MD_PLAY,0,1);
  int notes[64]={},seen=0;unsigned last_step=md_status()->step;for(int b=0;b<40000&&seen<64;b++){md_render(audio,64);if(md_status()->step!=last_step){last_step=md_status()->step;notes[last_step&63]=md_status()->last_note[0];seen++;}}
  cmd(MD_PLAY,0,0);int changes_off=0,changes_on=0;for(int k=1;k<63;k++)if(notes[k]&&notes[k-1]&&notes[k]!=notes[k-1])(k%4?changes_off:changes_on)++;assert(changes_off==0&&changes_on>3);
  puts("PASS Motion every N steps: held between beats, v9 migrates");
 }
 {
  // Review fixes. Leaving the internal clock while playing sends Stop and releases every note out.
  uint8_t m[3];md_patch q=blank(MD_VA);q.tempo=120;assert(md_load(&q));md_midi_routing r={};r.mode=1;r.clock=1;md_set_midi_routing(&r);
  cmd(MD_PLAY,0,1);run(200);while(md_midi_take(m));cmd(MD_CLOCK_SOURCE,0,1);bool stop=false;while(md_midi_take(m))stop|=m[0]==0xfc;assert(stop&&!md_playing());
  cmd(MD_CLOCK_SOURCE,0,0);
  // CLEAR_PHRASE: all rests, cursor on step 1, in one command.
  q=blank(MD_VA);q.ext[2].edit_step=9;assert(md_load(&q));cmd(MD_CLEAR_PHRASE,2,0);for(int k=0;k<16;k++)assert(md_state()->ext[2].notes[k]==-1);assert(md_state()->ext[2].edit_step==0);
  // A new Biome empties the generator registers, even while playing.
  q=blank(MD_VA);q.gen[0]={MD_GEN_RANDOM,1000,4,12};assert(md_load(&q));cmd(MD_PLAY,0,1);run(400);assert(md_status()->gen_valid[0]);cmd(MD_SCENE,0,3);assert(md_status()->gen_valid[0]==0);cmd(MD_PLAY,0,0);
  // TRACKS mode: the previous note on the channel is released before the bend changes.
  {md_patch t=blank(MD_VA);int rast=-1;for(int i=0;i<md_scale_count();i++)if(!strcmp(md_scale_name(i),"RAST"))rast=i;assert(rast>=0);t.scale=rast;t.track[0].note=63;assert(md_load(&t));
   md_midi_routing rr={};rr.mode=1;md_set_midi_routing(&rr);while(md_midi_take(m));cmd(MD_PLAY,0,1);bool held=false,bad=false;
   for(int b=0;b<400;b++){md_render(audio,128);while(md_midi_take(m)){if((m[0]&0xf0)==0x90)held=true;else if((m[0]&0xf0)==0x80)held=false;else if((m[0]&0xf0)==0xe0&&held)bad=true;}}
   cmd(MD_PLAY,0,0);assert(!bad);}
  puts("PASS review fixes: Stop on clock change, one-command phrase clear, Biome resets generators, bend never retunes a held note");
 }
 {
  // Dice, page by page: each roll changes something, stays valid, keeps scale / root / transpose / tempo / mutes / engines,
  // touches no other track, keeps the level except on the mixer, and ENV undoes it exactly.
  md_patch q=blank(MD_VA);q.root=4;q.scale=7;q.transpose=-5;q.tempo=97;q.track[2].level=640;q.track[2].mute=0;q.track[2].length=12;q.track[2].filter=0;assert(md_load(&q));
  for(int page=0;page<MD_DICE_PAGES;page++)for(int r=0;r<30;r++){md_patch before=*md_state();cmd(MD_DICE,2,page);const md_patch *p=md_state();assert(md_validate(p));
   assert(p->root==4&&p->scale==7&&p->transpose==-5&&p->tempo==97&&p->track[2].mute==0);
   for(int j=0;j<6;j++){assert(p->track[j].model==before.track[j].model);if(j!=2)assert(!memcmp(&p->track[j],&before.track[j],sizeof(md_track)));}
   if(page!=MD_DICE_MIXER)assert(p->track[2].level==before.track[2].level);else assert(p->track[2].level>=300);
   if(page==MD_DICE_FILTER)assert(p->track[2].filter>0&&(p->track[2].filter==4||p->track[2].filter==7||p->track[2].filter==5||p->track[2].cutoff>=400));
   if(page==MD_DICE_LFO_A){const md_lfo &l=p->lfo[2][0];assert(p->track[2].filter||(l.target!=MD_TO_CUTOFF&&l.target!=MD_TO_RESONANCE));assert(l.depth);if(l.target==MD_TO_PITCH)assert(abs(l.depth)<=300);}
   if(page==MD_DICE_SPACE||page==MD_DICE_SPACE_TONE||page==MD_DICE_ECHO)assert(p->fx_level>=250);
   if(page==MD_DICE_PHRASE)assert(p->ext[2].notes[0]>=0);
   assert(p->delay_fb<=600&&p->feedback<=600);
   assert(memcmp(p,&before,sizeof before));cmd(MD_RETURN,0,0);assert(!memcmp(md_state(),&before,sizeof before));cmd(MD_DICE,2,page);}
  // An effects roll is heard: a silent return comes up; with no send anywhere, the track gets echo and room.
  q=blank(MD_VA);q.fx_level=0;for(int i=0;i<6;i++){q.ext[i].delay_send=0;q.ext[i].reverb_send=0;}assert(md_load(&q));
  cmd(MD_DICE,4,MD_DICE_SPACE_TONE);assert(md_state()->fx_level>=400&&md_state()->ext[4].delay_send>0&&md_state()->ext[4].reverb_send>0&&md_state()->ext[1].delay_send==0);
  q.ext[1].reverb_send=300;q.fx_level=600;assert(md_load(&q));cmd(MD_DICE,4,MD_DICE_SPACE_TONE);assert(md_state()->fx_level==600&&md_state()->ext[4].delay_send==0);
  puts("PASS dice: every page in range, protected settings kept, other tracks untouched, ENV undoes, effects always heard");
 }
 {
  // Rhizome's Mondes: valid, audible, each with at least three bodies; DUSK keeps its orbits nearly round, NIGHT has
  // eccentric ones and moving ellipses, DAWN has moons, a comet or chords; all within 6 dB of each other but NO STARS.
  float level[MD_BIOMES]={};
  for(int id=0;id<MD_BLANK_BIOME;id++){cmd(MD_SCENE,0,id);const md_patch *p=md_state();assert(md_validate(p)&&p->biome_id==id&&p->version==13);
   int bodies=0,round=1,eccentric=0,moons=0;for(int i=0;i<6;i++){const md_orbit &o=p->orbit[i];if(!o.gates)continue;bodies++;round&=o.ecc<=250;eccentric+=o.ecc>=500;moons+=o.moons;}
   assert(bodies>=3);const char *fam=md_biome_family(id);
   if(!strcmp(fam,"DUSK"))assert(round);else if(!strcmp(fam,"NIGHT"))assert(eccentric>=2&&(p->orbit_prec||p->orbit_pert));
   else if(!strcmp(fam,"DAWN"))assert(moons||p->orbit_comet||p->orbit_conj==MD_CONJ_CHORD);
   else if(!strcmp(fam,"ECHO"))assert(p->fx_level>=500&&(p->tape_heads>1||p->hall_size>=850));
   else if(!strcmp(fam,"RAGA")){int g=0;for(int i=0;i<6;i++)g+=p->gamaka[i]!=0;assert(p->vadi>=0&&p->samvadi>=0&&g>0);} // pivot notes and ornaments
   else if(!strcmp(fam,"MAQAM")||!strcmp(fam,"EAST")){}
   else if(!strcmp(fam,"VOICE")){int own=0;for(int i=0;i<6;i++)own+=!p->track[i].mute&&p->track[i].model>=MD_SIXOP;assert(own==1);}
   else{assert(!strcmp(fam,"DEEP"));int lfos=0,gens=0,gamakas=0,free=0;for(int i=0;i<6;i++){lfos+=p->lfo[i][0].depth||p->lfo[i][1].depth;gens+=p->gen[i].source!=0;gamakas+=p->gamaka[i]!=0;free+=p->orbit[i].gates&&md_orbit_tenths(p->orbit[i].period);}
    assert(lfos||gens||gamakas||free||p->form||p->orbit_system);} // a long form: LFOs, generators, gamakas, free periods, the form or a real system
   cmd(MD_PLAY,0,1);double e=0;float peak=0;for(int b=0;b<20700;b++){md_render(audio,128);for(int k=0;k<256;k++){peak=fmaxf(peak,fabsf(audio[k]));e+=audio[k]*audio[k];}}cmd(MD_PLAY,0,0);
   level[id]=(float)(10*log10(e/(20700.0*256)));printf("  %-15s %-5s peak %.3f RMS %.1f dB\n",md_biome_name(id),fam,peak,level[id]);assert(peak>.01f&&peak<.95f);}
  for(int a=0;a<MD_BLANK_BIOME;a++)for(int b=0;b<MD_BLANK_BIOME;b++){auto sparse=[](int id){return !strcmp(md_biome_name(id),"NO STARS")||!strcmp(md_biome_name(id),"LOOPS OF ASH");};if(!sparse(a)&&!sparse(b))assert(fabsf(level[a]-level[b])<6);}
  puts("PASS Rhizome Mondes: 30 v13 patches, nine families, all audible and balanced");
  // The home page's macros act on the orbits: DENSITY adds or takes gates (a body keeps its orbit), MOTION moves notes.
  {cmd(MD_SCENE,0,0);md_patch q=*md_state();float phi[MD_ORBIT_GATES_MAX];int n[3];
   for(int k=0;k<3;k++){q.macro[0]=k*500;n[k]=md_orbit_gate_angles(&q,2,phi);}
   assert(n[0]<n[1]&&n[1]==q.orbit[2].gates&&n[2]>n[1]&&n[2]<=MD_ORBIT_GATES_MAX);
   q=*md_state();q.macro[1]=0;const uint64_t still=hash(q);q.macro[1]=600;assert(hash(q)!=still);
   q.macro[1]=0;q.gen[2]={MD_GEN_RANDOM,0,8,12};assert(hash(q)!=still); // and its generator moves a body's notes too
   puts("PASS DENSITY adds and takes gates, MOTION and the generator move orbit notes");}
  // A free period keeps its seconds whatever the tempo, even when the tempo changes on the way.
  {md_patch q=blank(MD_FM);for(int i=0;i<6;i++){q.orbit[i].gates=0;q.track[i].mute=i!=0;}
   q.orbit[0]={1,MD_ORBIT_TEMPO_PERIODS+3,0,0,MD_PITCH_ORBIT,0,1,MD_MOON_UP,60};q.tempo=140;assert(md_validate(&q)&&md_orbit_tenths(q.orbit[0].period)==178);
   md_init();assert(md_load(&q));cmd(MD_PLAY,0,1);uint32_t seen=md_status()->orbit_hits[0];double t=0,times[8];int n=0;
   for(int b=0;b<345*100&&n<6;b++){if(b==345*30)cmd(MD_TEMPO,0,60);md_render(audio,128);t+=128.0/44100;if(md_status()->orbit_hits[0]!=seen){seen=md_status()->orbit_hits[0];times[n++]=t;}}
   assert(n>=4);for(int k=1;k<n;k++)printf("  free 17.8 s orbit: %.3f s\n",times[k]-times[k-1]);for(int k=2;k<n;k++)assert(fabs(times[k]-times[k-1]-17.8)<.01); // the first turn after PLAY starts on the first step (up to one step off)
   puts("PASS a free period: 17.8 s a turn at 140 and at 60 BPM, through the change");}
  // The comet's FILTER: closed while it is far (AMOUNT 100: 280 Hz), wide open as it grazes the star, the mix's highs
  // following; stopped, wide open; another event, no filter at all.
  {md_patch q=*md_state();cmd(MD_SCENE,0,0);q=*md_state();q.orbit_comet=1;q.comet_event=MD_COMET_FILTER;q.comet_amount=100;for(int i=0;i<6;i++){q.track[i].filter=0;q.track[i].tone=800;} /* bright voices: the filter is what darkens them */assert(md_validate(&q));
   md_init();assert(md_load(&q));run(10);assert(fabsf(md_status()->comet_filter_hz-18000)<200); // stopped: open
   cmd(MD_PLAY,0,1);float lo=1e9f,hi=0;double hf[2]={},all[2]={};float prev=0;
   for(int b=0;b<345*20;b++){md_render(audio,128);const float hz=md_status()->comet_filter_hz;lo=fminf(lo,hz);hi=fmaxf(hi,hz);
    const int open=hz>4000;for(int k=0;k<256;k+=2){const float d=audio[k]-prev;prev=audio[k];hf[open]+=d*d;all[open]+=audio[k]*audio[k];}}
   printf("  comet filter: %.0f Hz far, %.0f Hz at the star; highs %.4f closed, %.4f open\n",lo,hi,hf[0]/all[0],hf[1]/all[1]);
   assert(lo<400&&hi>12000&&hf[1]/all[1]>4*hf[0]/all[0]);
   q.comet_event=MD_COMET_SWELL;md_init();assert(md_load(&q));cmd(MD_PLAY,0,1);run(400);assert(md_status()->comet_filter_hz==0);
   puts("PASS the comet's FILTER opens as it passes and closes far away; off for the other events");}
  // WANDER: the comet comes back at random (4 to 32 bars), each passage shocks bodies (more than one, at times: the
  // chain), the shocks wear off, nothing is written into the patch, and the same seed replays the same shocks.
  {cmd(MD_SCENE,0,0);md_patch q=*md_state();q.orbit_comet=MD_COMET_WANDER;q.comet_amount=100;q.tempo=180;assert(md_validate(&q));
   auto run_wander=[&](uint32_t *shocks,uint32_t *passes,int *gaps_seen){md_init();assert(md_load(&q));cmd(MD_PLAY,0,1);uint32_t seen=0;float last=-1;int distinct=0;float gaps[8]={};
    float burst=0;for(int b=0;b<345*240;b++){md_render(audio,128);const md_stats *st=md_status();burst=fmaxf(burst,st->shock_burst);if(st->comet_passes!=seen){seen=st->comet_passes;const float at=st->orbit_pos;
      if(last>=0&&distinct<8){gaps[distinct++]=at-last;}last=at;}}
    assert(burst>.9f); // the shocks jolt the effects (AMOUNT 100)
    int differ=0;for(int k=1;k<distinct;k++)differ+=fabsf(gaps[k]-gaps[k-1])>8;*gaps_seen=differ;*shocks=md_status()->shocks;*passes=md_status()->comet_passes;
    for(int i=0;i<6;i++)assert(md_status()->shock_left[i]<=5);assert(!memcmp(&md_state()->orbit,&q.orbit,sizeof q.orbit));};
   uint32_t s1,p1,s2,p2;int d1,d2;run_wander(&s1,&p1,&d1);run_wander(&s2,&p2,&d2);
   printf("  wander: %u passages, %u shocks, %d changes of return time\n",p1,s1,d1);
   assert(p1>=4&&s1>p1&&d1>=2&&s1==s2&&p1==p2); // chains: more shocks than passages; random returns; the same seed, the same
   puts("PASS WANDER: random returns, shocks and chains jolting the effects, temporary, reproducible from the seed");}
  // SIX-OP plays every note, not only the first: a bell (sustain 0) struck on every step stays audible (its single DX7
  // voice must see its gate fall between notes).
  {md_patch q=blank(MD_SIXOP);q.track[0].harm=557;q.track[0].decay=600;for(int i=0;i<6;i++)q.orbit[i].gates=0;assert(md_validate(&q)); // TUB BELLS
   md_init();assert(md_load(&q));cmd(MD_PLAY,0,1);double e[5]={};
   for(int b=0;b<345*5;b++){md_render(audio,128);for(int k=0;k<256;k++)e[b/345]+=audio[k]*audio[k];}
   printf("  SIX-OP repeated notes: energy %.3g first second, %.3g fifth\n",e[0],e[4]);assert(e[0]>0&&e[4]>.2*e[0]);
   puts("PASS SIX-OP strikes every note");}
  // Every voice builds in its arena with room to spare (the wavetable once wrote past it).
  {md_init();cmd(MD_SCENE,0,MD_BLANK_BIOME);for(int m=0;m<MD_MODELS;m++)for(int t=0;t<6;t++)cmd(MD_MODEL,t,m);
   printf("  voice arena: %u bytes to spare at least\n",md_voice_arena_spare());assert(md_voice_arena_spare()>0);puts("PASS every voice fits its arena");}
 }
 {
  // Morph LAST LIGHT (1) -> PALE COMET (10): numbers glide, choices switch half way, every step valid; ENV back to A.
  cmd(MD_SCENE,0,0);const md_patch A=*md_state();md_patch B;{cmd(MD_SCENE,0,9);B=*md_state();cmd(MD_SCENE,0,0);}
  md_morph_state ms;cmd(MD_MORPH_TO,0,9);md_morph_view(&ms,0,0);assert(ms.on&&ms.target==9&&ms.pos==0&&md_state()->tempo==A.tempo);
  for(int pos=0;pos<=1000;pos+=50){cmd(MD_MORPH,0,pos);const md_patch *p=md_state();assert(md_validate(p));
   assert(p->tempo==A.tempo+(int)floorf((float)(B.tempo-A.tempo)*(pos*.001f)+.5f));
   for(int j=0;j<6;j++)assert(p->track[j].model==(pos>=md_morph_point(j)?B.track[j].model:A.track[j].model));assert(p->scale==(pos>=500?B.scale:A.scale));}
  // Each track switches at its own point and is silent there, so its old engine has faded out; full level away from it.
  for(int j=0;j<6;j++){assert(md_morph_point(j)>md_morph_point(j>0?j-1:0)-1);if(A.track[j].model==B.track[j].model)continue;assert((md_morph_changes()>>j)&1);
   cmd(MD_MORPH,0,md_morph_point(j));assert(md_state()->track[j].level==0);cmd(MD_MORPH,0,md_morph_point(j)+100);
   assert(md_state()->track[j].level==A.track[j].level+(int)floorf((float)(B.track[j].level-A.track[j].level)*((md_morph_point(j)+100)*.001f)+.5f));}
  // REC in a fade keeps the track at its full level.
  {int j=0;while(j<6&&A.track[j].model==B.track[j].model)j++;assert(j<6);cmd(MD_MORPH,0,md_morph_point(j)+10);assert(md_state()->track[j].level<A.track[j].level/2+B.track[j].level/2);
   cmd(MD_MORPH_KEEP,0,0);assert(md_state()->track[j].level>0);cmd(MD_RETURN,0,0);cmd(MD_MORPH_TO,0,9);}
  cmd(MD_MORPH,0,1000);assert(md_state()->biome_id==9&&md_state()->lfo[0][0].depth==B.lfo[0][0].depth&&md_state()->gamaka[0]==B.gamaka[0]);
  cmd(MD_RETURN,0,0);assert(!memcmp(md_state(),&A,sizeof A));md_morph_view(&ms,0,0);assert(ms.on&&ms.pos==0);
  // Scope RHYTHM: the tempo moves, the engines stay on A.
  cmd(MD_MORPH_SCOPE,0,MD_MORPH_RHYTHM);cmd(MD_MORPH,0,1000);assert(md_state()->tempo==B.tempo&&md_state()->track[0].model==A.track[0].model&&md_state()->biome_id==-1);
  cmd(MD_MORPH_SCOPE,0,MD_MORPH_ALL);
  // PLAY carries it over 4 bars (64 steps), then REC keeps it; a new Monde ends a morph.
  cmd(MD_RETURN,0,0);cmd(MD_MORPH_BARS,0,1);cmd(MD_PLAY,0,1);while(md_status()->step<70)md_render(audio,128);
  md_morph_view(&ms,0,0);assert(ms.pos==1000&&md_state()->tempo==B.tempo);cmd(MD_PLAY,0,0);
  cmd(MD_MORPH_KEEP,0,0);md_morph_view(&ms,0,0);assert(!ms.on&&md_state()->tempo==B.tempo);
  cmd(MD_MORPH_TO,0,3);cmd(MD_SCENE,0,5);md_morph_view(&ms,0,0);assert(!ms.on);cmd(MD_MORPH_BARS,0,0);
  puts("PASS morph: glide, tracks switch one after the other inside a fade, valid all along, scope, 4-bar auto morph, keep, ENV back to A");
 }
 {
  // DERIVE from PALE COMET: 12 minutes of 8-bar glides. On a leash around home, scale / root / transpose never move,
  // tempo within 10 %, valid and audible all along; ENV back home exactly, REC a new home, OFF stops where it is.
  cmd(MD_SCENE,0,9);const md_patch H=*md_state();cmd(MD_DRIFT_AMOUNT,0,2);cmd(MD_DRIFT_SLOW,0,1);cmd(MD_PLAY,0,1);
  md_drift_state ds;md_drift_view(&ds,0);assert(ds.on&&ds.slow==1);int glides=0,last_pos=0,rests=0,notes=0;
  for(int sec=0;sec<720;sec++){float peak=0;for(int b=0;b<345;b++){md_render(audio,128);for(int k=0;k<256;k++)peak=fmaxf(peak,fabsf(audio[k]));}
   const md_patch *p=md_state();assert(md_validate(p)&&p->root==H.root&&p->scale==H.scale&&p->transpose==H.transpose);
   assert(p->tempo>=H.tempo-H.tempo/10-1&&p->tempo<=H.tempo+H.tempo/10+1);
   for(int j=0;j<6;j++){assert(abs(p->track[j].tone-H.track[j].tone)<=451&&abs(p->track[j].cutoff-H.track[j].cutoff)<=451);assert(p->track[j].level<=H.track[j].level);}
   if(sec%10==9)assert(peak>.002f&&peak<1.01f);
   md_drift_view(&ds,0);if(ds.pos<last_pos)glides++;last_pos=ds.pos;for(auto &e:ds.events){rests+=e.kind==2;notes+=e.kind==4;}}
  assert(glides>=10&&rests>0&&notes>0&&ds.seconds>700);
  cmd(MD_RETURN,0,0);assert(!memcmp(md_state(),&H,sizeof H));
  for(int b=0;b<4000;b++)md_render(audio,128);cmd(MD_DRIFT_HOME,0,0);const md_patch N=*md_state();cmd(MD_RETURN,0,0);assert(!memcmp(md_state(),&N,sizeof N));
  for(int b=0;b<4000;b++)md_render(audio,128);cmd(MD_DRIFT_SLOW,0,0);const md_patch S=*md_state();for(int b=0;b<4000;b++)md_render(audio,128);
  assert(!memcmp(md_state(),&S,sizeof S));md_drift_view(&ds,0);assert(!ds.on);
  // VOYAGE visits other Mondes and still keeps the scale; a new Monde ends the drift.
  cmd(MD_DRIFT_VOYAGE,0,2);cmd(MD_DRIFT_SLOW,0,1);bool visited=false;
  for(int b=0;b<345*240;b++){md_render(audio,128);if(b%345==0){assert(md_state()->scale==S.scale);md_drift_view(&ds,0);for(auto &e:ds.events)visited|=e.kind==5;}}
  assert(visited);cmd(MD_SCENE,0,3);md_drift_view(&ds,0);assert(!ds.on);cmd(MD_PLAY,0,0);cmd(MD_DRIFT_VOYAGE,0,0);
  puts("PASS derive: 12 minutes on a leash around home, scale kept, tempo within 10 %, rests and new notes, ENV home, REC new home, voyage");
 }
 {
  // Review 2: a knob turned during DERIVE stays and moves home; DICE during DERIVE lasts the glide and ENV still goes
  // home; a new Monde turns K2 back to OFF; a silent track stays silent; an unlocked seed keeps varying during a morph.
  md_drift_state ds;const md_patch *home;
  cmd(MD_SCENE,0,9);cmd(MD_DRIFT_SLOW,0,6);cmd(MD_PLAY,0,1);for(int b=0;b<2000;b++)md_render(audio,128);
  cmd(MD_CUTOFF,2,123);for(int b=0;b<3000;b++)md_render(audio,128);md_drift_view(&ds,&home);
  assert(md_state()->track[2].cutoff==123&&home->track[2].cutoff==123);
  const int tone_home=home->track[1].tone;cmd(MD_DICE,1,MD_DICE_SOUND);for(int b=0;b<500;b++)md_render(audio,128);
  md_drift_view(&ds,&home);assert(home->track[1].tone==tone_home);cmd(MD_RETURN,0,0);assert(!memcmp(md_state(),home,sizeof(md_patch)));
  cmd(MD_SCENE,0,10);md_drift_view(&ds,0);assert(!ds.on&&ds.slow==0);cmd(MD_PLAY,0,0);
  // The Blank Monde under a far drift: no track starts playing.
  cmd(MD_SCENE,0,MD_BLANK_BIOME);cmd(MD_DRIFT_AMOUNT,0,2);cmd(MD_DRIFT_SLOW,0,1);cmd(MD_PLAY,0,1);
  for(int sec=0;sec<90;sec++){for(int b=0;b<345;b++)md_render(audio,128);for(int j=0;j<6;j++)assert(md_state()->track[j].hits==0);}
  cmd(MD_DRIFT_SLOW,0,0);cmd(MD_PLAY,0,0);
  // A knob turned during an auto morph stays.
  cmd(MD_SCENE,0,0);cmd(MD_MORPH_TO,0,9);cmd(MD_MORPH_BARS,0,3);cmd(MD_PLAY,0,1);for(int b=0;b<1500;b++)md_render(audio,128);
  cmd(MD_RESONANCE,0,321);for(int b=0;b<1500;b++)md_render(audio,128);assert(md_state()->track[0].resonance==321);
  // Unlocked: the sequencer changes the seed every 32 steps and the morph no longer puts it back.
  cmd(MD_PLAY,0,0);cmd(MD_SCENE,0,0);cmd(MD_LOCK,0,0);const unsigned seed0=md_state()->seed;cmd(MD_MORPH_TO,0,9);cmd(MD_MORPH_BARS,0,5);cmd(MD_PLAY,0,1);
  while(md_status()->step<100)md_render(audio,128);assert(md_state()->seed!=seed0);cmd(MD_PLAY,0,0);cmd(MD_MORPH_BARS,0,0);
  puts("PASS review 2: knobs stay during DERIVE and auto morph, DICE keeps home, OFF after a new Monde, silent tracks stay silent, unlocked seed varies");
 }
 {
  // External review: loading a patch ends a morph and a drift; an overload is not an edit and, in ECO, sheds effects;
  // the same clock source changes nothing; PANIC sends All Notes Off; clock out switched while playing sends Start;
  // a malformed LFO track and a huge MIDI note are ignored or bounded.
  md_morph_state ms;md_drift_state ds;cmd(MD_SCENE,0,9);const md_patch P=*md_state();
  cmd(MD_MORPH_TO,0,0);cmd(MD_DRIFT_SLOW,0,0);assert(md_load(&P));md_morph_view(&ms,0,0);assert(!ms.on);
  cmd(MD_DRIFT_SLOW,0,2);assert(md_load(&P));md_drift_view(&ds,0);assert(!ds.on&&ds.slow==0);
  {md_patch q=P;q.quality=1;q.edited_mask=0;assert(md_load(&q));const unsigned before=md_status()->quality_reductions;
   cmd(MD_OVERLOAD,0,0);assert(md_state()->quality==0&&md_state()->edited_mask==0&&md_status()->quality_reductions==before+1);
   cmd(MD_OVERLOAD,0,0);assert(md_state()->edited_mask==0&&md_status()->quality_reductions==before+2);
   cmd(MD_QUALITY,0,1);assert(md_state()->edited_mask==0);cmd(MD_QUALITY,0,0);}
  uint8_t m[3];md_midi_routing r={};r.mode=1;r.clock=1;md_set_midi_routing(&r);cmd(MD_PLAY,0,1);run(100);while(md_midi_take(m));
  cmd(MD_CLOCK_SOURCE,0,0);assert(md_playing());
  cmd(MD_PANIC,0,0);int cc123=0;while(md_midi_take(m))if((m[0]&0xf0)==0xb0&&m[1]==123)cc123|=1<<(m[0]&15);assert(cc123==0x3f);
  r.clock=0;md_set_midi_routing(&r);cmd(MD_PLAY,0,1);run(50);while(md_midi_take(m));r.clock=1;md_set_midi_routing(&r);
  bool start=false;while(md_midi_take(m))start|=m[0]==0xfa;assert(start);cmd(MD_PLAY,0,0);r.mode=0;r.clock=0;md_set_midi_routing(&r);
  const md_patch L=*md_state();cmd(MD_LFO_DEPTH,12,500);cmd(MD_LFO_DEPTH,-1,500);assert(!memcmp(md_state()->lfo,L.lfo,sizeof L.lfo));
  cmd(MD_TRIGGER,0,2000000000);cmd(MD_TRIGGER,0,-2000000000);run(10);
  puts("PASS external review: load ends morph and drift, overload not an edit, same clock keeps playing, PANIC CC 123, clock Start, bounds");
 }
 {
  // Channels per track: tracks 1-6 hear channels 3-8 and send on 3, 4, OFF, 6, 7, 8 (an MS-20 on 1, a 2600 on 2).
  md_patch q=blank(MD_VA);for(int j=0;j<6;j++){q.track[j].mute=0;q.track[j].prob=1000;q.track[j].hits=4;q.track[j].length=16;}assert(md_load(&q));
  uint8_t m[3];const int32_t in[6]={3,4,5,6,7,8},out[6]={3,4,0,6,7,8};md_set_midi_channels(in,out);
  unsigned before=md_status()->events[0];md_midi(0x90,60,100,0);md_midi(0x80,60,0,0);run(2);assert(md_status()->events[0]==before); // channel 1 is the MS-20's
  md_midi(0x92,60,100,0);run(2);assert(md_status()->events[0]==before+1&&md_status()->last_note[0]==60);md_midi(0x82,60,0,0);
  before=md_status()->events[5];md_midi(0x97,62,100,0);run(2);assert(md_status()->events[5]==before+1);md_midi(0x87,62,0,0);
  const int32_t shared[6]={3,3,5,6,7,8};md_set_midi_channels(shared,out);unsigned b0=md_status()->events[0],b1=md_status()->events[1];
  md_midi(0x92,64,100,0);run(2);assert(md_status()->events[0]==b0+1&&md_status()->events[1]==b1+1);md_midi(0x82,64,0,0); // two tracks, one channel
  md_midi(0xbf,16,127,0);run(1);assert(md_state()->macro[0]==1000); // channel 16, CC 16-19: the macros
  md_set_midi_channels(in,out);md_midi_routing r={};r.mode=1;md_set_midi_routing(&r);while(md_midi_take(m));
  int on=0;cmd(MD_PLAY,0,1);for(int b=0;b<600;b++){md_render(audio,128);while(md_midi_take(m))if((m[0]&0xf0)==0x90)on|=1<<(m[0]&15);}
  assert(on==((1<<2)|(1<<3)|(1<<5)|(1<<6)|(1<<7))); // channels 3, 4, 6, 7, 8; track 3 is OFF; never 1 or 2
  cmd(MD_PANIC,0,0);int cc123=0;while(md_midi_take(m))if((m[0]&0xf0)==0xb0&&m[1]==123)cc123|=1<<(m[0]&15);assert(cc123==on);
  cmd(MD_PLAY,0,0);r.mode=0;md_set_midi_routing(&r);const int32_t dflt[6]={1,2,3,4,5,6};md_set_midi_channels(dflt,dflt);
  puts("PASS MIDI channels per track: in, out, OFF, shared, macros on 16, PANIC on the channels used");
 }
 {
  // LFO on ROTATE: S&H synced to the bar turns the kick's pattern to a new place each bar, its hits unchanged.
  md_patch q=blank(MD_KICK);q.track[0].hits=4;q.track[0].length=16;q.track[0].rotate=0;q.track[0].prob=1000;q.tempo=120;assert(md_load(&q));
  auto bars=[&](int n){std::vector<int>rot;for(int b=0;b<n;b++){run(689);rot.push_back(md_status()->rotate_now[0]);assert(md_status()->hits_now[0]==4);}return rot;}; // one bar at 120 BPM: 2 s, 689 blocks of 128
  cmd(MD_PLAY,0,1);for(int r:bars(6))assert(r==0); // no LFO: the knob's rotation
  cmd(MD_PLAY,0,0);cmd(MD_LFO_RATE,0,3);cmd(MD_LFO_SHAPE,0,MD_LFO_SAMPLE_HOLD);cmd(MD_LFO_TARGET,0,MD_TO_ROTATE);cmd(MD_LFO_DEPTH,0,1000);
  cmd(MD_PLAY,0,1);auto rot=bars(12);int moved=0;for(size_t k=1;k<rot.size();k++){assert(rot[k]>=0&&rot[k]<16);moved+=rot[k]!=rot[k-1];}
  assert(moved>=4); // S&H draws a new rotation each bar
  cmd(MD_LFO_DEPTH,0,0);run(689);assert(md_status()->rotate_now[0]==0);cmd(MD_PLAY,0,0);
  assert(!strcmp(md_lfo_target_name(MD_TO_ROTATE),"ROTATE"));
  puts("PASS LFO on ROTATE: S&H to the bar turns the pattern, hits unchanged, depth 0 back to the knob");
 }

 { // Rhizome orbits
  // Plays a patch for `turns` turns of track 0's orbit, block by block (64 samples), and notes each gate it plays.
  auto play_orbit=[](const md_patch &p,int turns,std::vector<float> &at,std::vector<int> *notes=nullptr){
   md_init();assert(md_load(&p));assert(md_command(MD_PLAY,0,1));
   uint32_t seen=md_status()->orbit_hits[0];float buf[128];const float end=(float)(md_orbit_steps(p.orbit[0].period)*turns)-.01f;
   for(int guard=0;md_status()->orbit_pos<end&&guard<200000;guard++){md_render(buf,64);
    if(md_status()->orbit_hits[0]!=seen&&md_status()->orbit_hit_pos[0]<end){seen=md_status()->orbit_hits[0];at.push_back(md_status()->orbit_hit_pos[0]);if(notes)notes->push_back(md_status()->last_note[0]);}}
  };
  md_patch p=blank(MD_VA);p.tempo=120;
  // A round orbit is the euclidean rhythm, exactly: E(5,16) on a one-bar turn.
  p.orbit[0]={5,4,0,0,MD_PITCH_ORBIT};
  {std::vector<float> at;play_orbit(p,4,at);assert(at.size()==20);static const int e516[5]={0,4,7,10,13};
   for(size_t k=0;k<at.size();k++)assert(fabsf(at[k]-(float)(16*(k/5)+e516[k%5]))<.05f);}
  // An eccentric one keeps the count and the loop, with uneven gaps (rubato).
  p.orbit[0].ecc=600;
  {std::vector<float> at;play_orbit(p,4,at);assert(at.size()==20);float lo=99,hi=0;
   for(size_t k=1;k<5;k++){float d=at[k]-at[k-1];lo=fminf(lo,d);hi=fmaxf(hi,d);}assert(hi-lo>1.0f);
   for(size_t k=5;k<at.size();k++)assert(fabsf(at[k]-at[k-5]-16)<.05f);}
  // Gates 0: the track is back on its euclidean pattern (no orbit hit).
  p.orbit[0].gates=0;{std::vector<float> at;md_patch q=p;q.orbit[0].period=4;play_orbit(q,2,at);assert(at.empty());}
  // MOTIF walks the phrase in order, rests skipped.
  p.orbit[0]={3,4,0,0,MD_PITCH_MOTIF};for(int k=0;k<16;k++)p.ext[0].notes[k]=-1;p.ext[0].notes[0]=0;p.ext[0].notes[5]=7;p.ext[0].notes[9]=12;
  {std::vector<float> at;std::vector<int> n;play_orbit(p,3,at,&n);assert(n.size()==9);
   for(size_t k=3;k<n.size();k++)assert(n[k]==n[k-3]);assert(n[0]<n[1]&&n[1]<n[2]);
   assert(md_status()->motif_at[0]==1);} // the gate at the very end of the third turn played step 0 again: it looks from step 1
  // Two bodies at different speeds meet (a conjunction) and the counter says so.
  p.orbit[0]={1,4,0,0,MD_PITCH_ORBIT};p.track[1]=p.track[0];p.orbit[1]={1,5,0,0,MD_PITCH_ORBIT};
  {std::vector<float> at;play_orbit(p,7,at);assert(md_status()->conj_count>=1);}
  // CHORD: at a conjunction both bodies strike again together, on top of their gates.
  {md_patch q=p;q.orbit_conj=MD_CONJ_CHORD;std::vector<float> at;play_orbit(q,7,at);const md_stats *s=md_status();
   assert(s->conj_count>=1&&s->events[0]+s->events[1]>=s->orbit_hits[0]+s->orbit_hits[1]+2*s->conj_count);
   q.orbit_conj=MD_CONJ_ACCENT;at.clear();play_orbit(q,7,at);s=md_status();assert(s->events[0]+s->events[1]==s->orbit_hits[0]+s->orbit_hits[1]);}
  // The comet: starts far away, passes the star half a turn later, acts about a fifth of its turn, freezes the tape.
  {md_patch q=p;q.orbit_comet=1;q.comet_event=MD_COMET_BOTH;q.comet_amount=100;assert(md_orbit_comet_steps(1)==64);
   md_init();assert(md_load(&q));assert(md_command(MD_PLAY,0,1));float buf[128],peak=0,peak_at=-1;int acting=0,blocks=0,frozen=0;
   for(int guard=0;md_status()->orbit_pos<127.9f&&guard<400000;guard++){md_render(buf,64);const md_stats *s=md_status();blocks++;
    if(s->comet_now>0)acting++;if(s->comet_now>peak){peak=s->comet_now;peak_at=s->orbit_pos;}
    if(s->comet_now>.5f){md_tape t;md_tape_view(&t);frozen+=t.freeze;}}
   assert(md_status()->comet_passes==2&&peak>.99f&&fabsf(peak_at-32)<.5f&&frozen>0);
   const float share=(float)acting/blocks;assert(share>.12f&&share<.25f);
   md_command(MD_PLAY,0,0);for(int b=0;b<20;b++)md_render(buf,64);md_tape t;md_tape_view(&t);assert(!t.freeze);}
  {md_patch bad=p;bad.orbit_comet=MD_COMETS;assert(!md_validate(&bad));bad=p;bad.orbit_conj=MD_CONJ_MODES;assert(!md_validate(&bad));}
  // Real systems: KEPLER-223 on the unmuted melodic tracks, its 3:4:6:8 heard as a polyrhythm (same gates on each body).
  {md_patch q=p;for(int i=1;i<5;i++){q.track[i]=q.track[0];q.orbit[i].gates=0;}q.track[0].mute=1;q.orbit[0].gates=0;q.orbit[1].gates=0;
   md_init();assert(md_load(&q));assert(md_command(MD_ORBIT_SYSTEM,0,2));float buf[128];md_render(buf,64);const md_patch *s=md_state();
   assert(s->orbit_system==2&&s->orbit[0].gates==0&&!strcmp(md_orbit_system_name(2,0),"KEPLER-223"));
   static const int steps[4]={48,64,96,128},ecc[4]={78,150,37,51};
   for(int b=0;b<4;b++)assert(md_orbit_steps(s->orbit[b+1].period)==steps[b]&&s->orbit[b+1].ecc==ecc[b]&&s->orbit[b+1].gates==4);
   assert(md_command(MD_PLAY,0,1));for(int guard=0;md_status()->orbit_pos<383.9f&&guard<2000000;guard++)md_render(buf,64);
   for(int b=0;b<4;b++)assert(md_status()->orbit_hits[b+1]==(uint32_t)(4*384/steps[b]));
   assert(md_command(MD_ORBIT_ECC,1,100));md_render(buf,64);assert(md_state()->orbit_system==0); // edited: your own
   md_command(MD_PLAY,0,0);md_render(buf,64);assert(md_command(MD_ORBIT_SYSTEM,0,4));md_render(buf,64);s=md_state(); // PLUTO: Charon
   assert(md_orbit_steps(s->orbit[1].period)==64&&md_orbit_steps(s->orbit[2].period)==96&&s->orbit[2].ecc==249&&s->orbit[2].moons==1&&s->orbit[3].gates==0);
   md_patch bad=*s;bad.orbit_system=MD_ORBIT_SYSTEMS;assert(!md_validate(&bad));}
  // The tape echo: a pluck comes back on each head in use, at its share of TIME; FREEZE loops the tape; STOP brakes it.
  {auto echoes=[](int heads,int ms){md_patch q=blank(MD_BASS);q.track[0].decay=0;q.ext[0].delay_send=1000;q.ext[0].reverb_send=0;
    q.fx_level=1000;q.delay_fb=0;q.delay_ms=ms;q.echo_sync=0;q.tape_heads=heads;md_init();assert(md_load(&q));float buf[128];
    for(int b=0;b<700;b++)md_render(buf,64); // the tape reaches its speed
    assert(md_command(MD_TRIGGER,0,48));std::vector<double> e;for(int b=0;b<1500;b++){md_render(buf,64);double s=0;for(float x:buf)s+=x*x;e.push_back(s);}
    return e;};
   auto win=[](const std::vector<double> &e,int t0,int t1){double s=0;for(int b=t0*441/640;b<t1*441/640;b++)s+=e[b];return s;}; // ms to 64-sample blocks
   {auto e=echoes(1,300);assert(win(e,300,330)>30*win(e,250,285));}
   {auto e=echoes(7,600);for(int t:{200,400,600})assert(win(e,t,t+30)>10*win(e,t-50,t-15));}
   md_reel r;md_reel_view(&r);assert(r.heads==7&&fabsf(r.speed-31500/(600*22.05f))<.05f);}
  {md_patch q=blank(MD_BASS);q.track[0].decay=0;q.ext[0].delay_send=1000;q.fx_level=1000;q.delay_fb=0;q.delay_ms=500;q.tape_heads=1;
   md_init();assert(md_load(&q));float buf[128];auto energy=[&](int blocks){double s=0;for(int b=0;b<blocks;b++){md_render(buf,64);for(float x:buf)s+=x*x;}return s;};
   energy(700);assert(md_command(MD_TRIGGER,0,48));energy(100);assert(md_command(MD_TAPE_TRANSPORT,0,MD_TAPE_FREEZE));
   energy(2000);assert(energy(1400)>1e-3); // two seconds later, without feedback, the loop still plays the pluck
   assert(md_command(MD_TAPE_TRANSPORT,0,MD_TAPE_STOP));energy(3000);md_reel r;md_reel_view(&r);assert(r.speed<.01f&&r.transport==MD_TAPE_STOP&&energy(700)<1e-6);
   assert(md_command(MD_TAPE_TRANSPORT,0,MD_TAPE_RUN));energy(3000);md_reel_view(&r);assert(fabsf(r.speed-10500/(500*22.05f))<.05f);
   // REWIND: the tape runs back three times as fast, the pluck comes back reversed, nothing is recorded; RUN again.
   assert(md_command(MD_TRIGGER,0,48));energy(100);const uint32_t at=r.pos;(void)at;assert(md_command(MD_TAPE_TRANSPORT,0,MD_TAPE_REWIND));energy(3000);md_reel_view(&r);
   assert(r.transport==MD_TAPE_REWIND&&fabsf(r.speed+3*10500/(500*22.05f))<.1f);
   {const float rec=r.rec_peak;double back=energy(2000);md_reel_view(&r);assert(back>1e-4&&r.rec_peak<=rec+1e-6f);} // heard, reversed; the record head stays up
   assert(md_command(MD_TAPE_TRANSPORT,0,MD_TAPE_RUN));energy(3000);md_reel_view(&r);assert(fabsf(r.speed-10500/(500*22.05f))<.05f);
   md_patch bad=q;bad.tape_heads=0;assert(!md_validate(&bad));bad=q;bad.delay_ms=MD_TAPE_MS_MAX+1;assert(!md_validate(&bad));}
  // The HALL: the tail falls 60 dB in the time DECAY asks for (measured between -10 and -40 dB), at 44.1 kHz and in
  // VINTAGE; at the top it holds.
  {assert(fabsf(md_hall_rt60(376)-2.0f)<.02f&&md_hall_rt60(1000)<0);
   auto tail=[](int decay,int vintage,int seconds,std::vector<double> &e){md_patch q=blank(MD_SNARE); // noise: all frequencies (a tone sits on the allpasses' combs)
    q.track[0].decay=0;q.ext[0].delay_send=0;q.ext[0].reverb_send=1000;
    q.fx_level=1000;q.reverb_time=decay;q.hall_bass=500;q.room_tone=1000;q.hall_predelay=0;q.hall_vintage=vintage;md_init();assert(md_load(&q));float buf[128];
    assert(md_command(MD_TRIGGER,0,48));for(int b=0;b<seconds*44100/(64*20);b++){double s=0;for(int j=0;j<20;j++){md_render(buf,64);for(float x:buf)s+=x*x;}e.push_back(s);}}; // 29 ms windows
   for(int vintage=0;vintage<2;vintage++){std::vector<double> e;tail(376,vintage,4,e);
    auto at=[&](double t){size_t k=(size_t)(t*44100/1280);double s=0;for(size_t j=k;j<k+4;j++)s+=e[j];return s;};
    const double rt=60/(10*log10(at(.3)/at(1.0))*(1/.7)); // the tail alone (the dry note is gone), above the 16-bit floor
    printf("HALL decay 2.0 s%s: RT60 measured %.2f s\n",vintage?" (vintage)":"",rt);assert(rt>1.5&&rt<2.6);}
   std::vector<double> e;tail(1000,0,0,e);md_patch q=*md_state();q.reverb_time=700;e.clear();tail(700,0,2,e);
   assert(md_command(MD_REVERB_TIME,0,1000));float buf[128];double s1=0,s2=0;for(int b=0;b<700;b++)md_render(buf,64);
   for(int b=0;b<700;b++){md_render(buf,64);for(float x:buf)s1+=x*x;}for(int b=0;b<2000;b++)md_render(buf,64);for(int b=0;b<700;b++){md_render(buf,64);for(float x:buf)s2+=x*x;}
   assert(s1>1e-6&&s2>s1*.5); // infinite: three seconds on, the tail is still there
   md_patch bad=*md_state();bad.hall_predelay=251;assert(!md_validate(&bad));bad=*md_state();bad.reverb_time=1001;assert(!md_validate(&bad));}
  // The grain bus (Monades' grains since 4 November 2026): each media gives its memory; DENSITY in the middle makes no grain, either side makes some; FREEZE keeps
  // playing what is in memory after the tracks fall silent; SPACE at 0 rests (nothing recorded, nothing computed).
  {static const float secs[MD_BEADS_MEDIAS]={2.972f,2.972f,1.486f}; // DIRTY TAPE, CLEAN TAPE (half rate), DIGITAL: Monades' grains, 64 Ki samples
   md_patch q=blank(MD_FM);q.track[0].send=1000;q.macro[3]=800;q.tempo=120;q.beads_density=900;float buf[128];
   for(int m=0;m<MD_BEADS_MEDIAS;m++){md_patch r=q;r.beads_media=m;md_init();assert(md_load(&r));md_render(buf,64);assert(fabsf(md_beads_seconds()-secs[m])<.02f);}
   auto most_grains=[&](int density){md_patch r=q;r.beads_media=MD_BEADS_DIGITAL;r.beads_density=density;md_init();assert(md_load(&r));assert(md_command(MD_PLAY,0,1));int most=0;
    for(int b=0;b<2000;b++){md_render(buf,64);if(b%25==0){md_tape t;md_tape_view(&t);int n=0;for(int j=0;j<MD_TAPE_GRAINS;j++)n+=t.grain[j].active;if(n>most)most=n;}}return most;};
   assert(most_grains(500)==0&&most_grains(900)>=2&&most_grains(150)>=1);
   auto after=[&](bool frozen){md_patch r=q;r.beads_media=MD_BEADS_DIGITAL;md_init();assert(md_load(&r));assert(md_command(MD_PLAY,0,1));for(int b=0;b<1400;b++)md_render(buf,64);
    assert(md_command(MD_MUTE,0,1));if(frozen)assert(md_command(MD_FREEZE,0,1));for(int b=0;b<1400;b++)md_render(buf,64); // two seconds: the memory has turned over
    double e=0;for(int b=0;b<700;b++){md_render(buf,64);for(float x:buf)e+=x*x;}return e;};
   const double still=after(true),gone=after(false);assert(still>1e-4&&still>20*gone);
   {md_patch r=q;r.macro[3]=0;md_init();assert(md_load(&r));assert(md_command(MD_PLAY,0,1));const uint32_t w=md_status()->grain_writes;for(int b=0;b<500;b++)md_render(buf,64);assert(md_status()->grain_writes==w);}
   md_patch bad=q;bad.beads_media=MD_BEADS_MEDIAS;assert(!md_validate(&bad));bad=q;bad.beads_density=1001;assert(!md_validate(&bad));}
  // A new period while playing keeps the body where it is: no burst of gates.
  p.orbit[1].gates=0;p.orbit[0]={16,4,300,0,MD_PITCH_ORBIT};
  {md_init();assert(md_load(&p));assert(md_command(MD_PLAY,0,1));float buf[128];for(int b=0;b<400;b++)md_render(buf,64);
   const uint32_t before=md_status()->orbit_hits[0];assert(md_command(MD_ORBIT_PERIOD,0,12));md_render(buf,64);md_render(buf,64);
   assert(md_status()->orbit_hits[0]-before<=1);}
  // Moons: each note spins them once; moon k plays k/(n+1) of the spin after it, quieter, and leaves the note in place.
  p.orbit[0]={4,4,0,0,MD_PITCH_ORBIT,3,3,MD_MOON_UP,80}; // a note every beat, three moons spun in a beat
  {md_init();assert(md_load(&p));assert(md_command(MD_PLAY,0,1));float buf[128];std::vector<float> notes,moons;uint32_t ns=0,ms=0;int body=-1;
   for(int guard=0;md_status()->orbit_pos<31.9f&&guard<200000;guard++){md_render(buf,64);const md_stats *s=md_status();
    if(s->orbit_hits[0]!=ns){ns=s->orbit_hits[0];notes.push_back(s->orbit_hit_pos[0]);body=s->last_note[0];}
    if(s->moon_hits[0]!=ms){ms=s->moon_hits[0];moons.push_back(s->orbit_pos);assert(s->last_note[0]==body);}}
   assert(notes.size()==8&&moons.size()==24);
   for(size_t k=0;k<moons.size();k++)assert(fabsf(moons[k]-(notes[k/3]+(float)(k%3+1)))<.05f);
   const md_stats *s=md_status();assert(s->last_pitch[0]<s->moon_note[0][0]&&s->moon_note[0][0]<s->moon_note[0][1]&&s->moon_note[0][1]<s->moon_note[0][2]);}
  p.orbit[0].moon_interval=MD_MOON_HARMONICS; // the octave first, two octaves last
  {md_init();assert(md_load(&p));assert(md_command(MD_PLAY,0,1));float buf[128];for(int b=0;b<200;b++)md_render(buf,64);
   const md_stats *s=md_status();assert(s->moon_t0[0]>-100&&fabsf(s->moon_note[0][0]-s->last_pitch[0]-12)<.01f&&fabsf(s->moon_note[0][2]-s->last_pitch[0]-24)<.01f);}
  p.orbit[0].moon_interval=MD_MOON_UP;
  p.orbit[0].moon_spin=0; // a sixteenth: a quick triplet of ornaments inside the step
  {md_init();assert(md_load(&p));assert(md_command(MD_PLAY,0,1));float buf[128];std::vector<float> moons;uint32_t ms=0;
   for(int guard=0;md_status()->orbit_pos<3.9f&&guard<200000;guard++){md_render(buf,64);if(md_status()->moon_hits[0]!=ms){ms=md_status()->moon_hits[0];moons.push_back(md_status()->orbit_pos);}}
   assert(moons.size()==3);for(int k=0;k<3;k++)assert(fabsf(moons[k]-.25f*(k+1))<.03f);}
  {md_patch bad=p;bad.orbit[0].moons=MD_MOONS_MAX+1;assert(!md_validate(&bad));bad=p;bad.orbit[0].moon_interval=MD_MOON_INTERVALS;assert(!md_validate(&bad));}
  // Monades v10 patches load with the orbits off.
  {md_init();md_patch q=*md_state();q.version=10;assert(md_import(&q,offsetof(md_patch,orbit)));assert(md_state()->version==13);
   for(int i=0;i<6;i++)assert(md_state()->orbit[i].gates==0);
   md_patch bad=*md_state();bad.orbit[2].ecc=MD_ORBIT_ECC_MAX+1;assert(!md_validate(&bad));}
  // Rhizome v12 patches (its eight Plaits voices) come back with the nearest of today's fourteen, and no library sound;
  // v11 (Monades' voices) as they were.
  {md_init();md_patch q=*md_state();q.version=12;const int v12[6]={0,1,2,3,4,7},now[6]={MD_STRINGS,MD_SATIN,MD_MELLOW,MD_SIXOP,MD_VA,MD_HARMONIC};
   for(int i=0;i<6;i++){q.track[i].model=v12[i];q.tone_preset[i]=5;}assert(md_import(&q,sizeof q));
   for(int i=0;i<6;i++)assert(md_state()->track[i].model==now[i]&&md_state()->tone_preset[i]==-1);assert(md_state()->version==13);
   q.version=11;for(int i=0;i<6;i++)q.track[i].model=MD_ELEMENTS;assert(md_import(&q,sizeof q));assert(md_state()->track[3].model==MD_ELEMENTS&&md_state()->version==13);
   q.version=12;q.track[0].model=9;assert(!md_import(&q,sizeof q));
   puts("PASS v12 patches: Rhizome's Plaits voices mapped to the nearest; v11 kept");}
  puts("PASS orbits: round = euclidean, eccentric = rubato on the same loop, MOTIF order, conjunction, no burst, moons, comet, chord, real systems, tape echo heads / freeze / stop, hall RT60 / vintage / infinite, beads media / density / freeze / rest, v10 import");
 }
 puts("ALL NEXT FEATURE TESTS PASSED; no hardware-performance claim");
}
