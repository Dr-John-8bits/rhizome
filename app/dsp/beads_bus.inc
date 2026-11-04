/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the granular bus. Beads (Mutable Instruments) took 50 to 70 % of the FM-1's processor as soon as sound went
 * through it (measured on the device, 4 November 2026), so the grains are now Monades' own, much lighter: up to four
 * grains reading a tape of the grain sends, each in a window. Included by dreams.cpp.
 * The page and the patch keep their fields: POSITION (where behind the write head grains start), GRAIN SIZE (50 to 300
 * ms), PITCH, DIRECTION (reverse), FEEDBACK, FREEZE (the comet's too); DENSITY (below the middle, regular grains, more
 * toward 0; above, random ones, more toward 1000; the middle, none), SHAPE (the window, drawn on the page), RANDOM
 * (position, pan and pitch spread), MEDIA: DIGITAL (16 bits, 1.5 s), CLEAN TAPE (half rate, 3 s), DIRTY TAPE (half rate,
 * coarse 8-bit steps and a slow wobble, 3 s). The SPACE macro sets their level; at 0 the bus rests. */
enum { GR_N=65536, GR_M=GR_N-1, GR_MAX=4 };
static int16_t gr_tape[GR_N] POOL; // 128 KiB, as before
struct GrainVoice { float pos,rate,pan,gain,inc; unsigned life,age; };
static GrainVoice gr[GR_MAX];
static unsigned gr_head,gr_valid,gr_sub,gr_rng=0x9e3779b9u;static float gr_clock,gr_fb,gr_wobble;
static uint8_t tape_peak[256];static unsigned tape_peak_col;
static int gr_media_now=-1;
static unsigned gr_step(){return patch.beads_media==MD_BEADS_DIGITAL?1:2;} // tape samples are written every step outputs
static float gr_noise(){gr_rng=gr_rng*1664525u+1013904223u;return (float)(int32_t)gr_rng*(1.0f/2147483648.0f);}
static void beads_reset(){memset(gr_tape,0,sizeof gr_tape);memset(gr,0,sizeof gr);gr_head=gr_valid=gr_sub=0;gr_clock=0;gr_fb=gr_wobble=0;gr_rng=0x9e3779b9u;
 memset(tape_peak,0,sizeof tape_peak);tape_peak_col=0;gr_media_now=-1;}
static void beads_init(){beads_reset();}
extern "C" float md_beads_rate(int density);
extern "C" float md_beads_envelope(float shape,float t);
static float gr_read(float p){ // linear, between tape samples (p in tape samples, may be negative)
 const int i=(int)(p+GR_N)-GR_N;const float f=p-(float)i;
 return (gr_tape[i&GR_M]+(gr_tape[(i+1)&GR_M]-gr_tape[i&GR_M])*f)*(1.0f/32768.0f);
}
// One block: the grain sends in, the grains out (wet only).
static void beads_block(const float *send,unsigned n,float *wl,float *wr,bool frozen){
 if(!patch.macro[3]&&smooth[3]<.0005f){for(unsigned k=0;k<n;k++)wl[k]=wr[k]=0;return;} // SPACE at 0: the bus rests
 if(gr_media_now!=patch.beads_media){gr_media_now=patch.beads_media;memset(gr_tape,0,sizeof gr_tape);gr_valid=0;memset(gr,0,sizeof gr);memset(tape_peak,0,sizeof tape_peak);}
 const unsigned step=gr_step();const bool dirty=patch.beads_media==MD_BEADS_DIRTY_TAPE;
 const float rnd=patch.beads_random*.001f,shape=patch.beads_shape*.001f,fb=unit(patch.feedback)*.9f;
 const float size=clamp01(unit(patch.grain_size)+lfo_grain_size*.5f),pos01=clamp01(unit(patch.grain_pos)+lfo_grain_pos*.5f);
 const float rate=md_beads_rate(patch.beads_density),density=patch.beads_density*.001f; // grains a second
 const float pitch=stmlib::SemitonesToRatio((float)patch.grain_pitch),dir=patch.reverse?-1.0f:1.0f;
 gr_wobble+=n*(.7f/MD_RATE);if(gr_wobble>=1)gr_wobble-=1;const float wob=dirty?1+.004f*plaits::Sine(gr_wobble):1;
 for(unsigned k=0;k<n;k++){
  // A new grain: regular below the middle of DENSITY, random above; only into a free slot.
  bool start=false;
  if(rate>0){if(density<.5f){gr_clock+=rate*(1.0f/MD_RATE);if(gr_clock>=1){gr_clock-=1;start=true;}}else start=(gr_noise()*.5f+.5f)<rate*(1.0f/MD_RATE);}
  if(start&&gr_valid>GR_N/8){int slot=-1;for(int j=0;j<GR_MAX;j++)if(gr[j].age>=gr[j].life){slot=j;break;}
   if(slot>=0){GrainVoice &g=gr[slot];g.life=2205+(unsigned)(size*11025);g.age=0;g.inc=1.0f/g.life;
    g.rate=dir*pitch*stmlib::SemitonesToRatio(rnd*1.5f*gr_noise())/step; // tape samples per output sample
    const float span=(float)gr_valid-512-fabsf(g.rate)*g.life,behind=256+fmaxf(0.0f,(g.rate-1.0f/step)*g.life)+fmaxf(0.0f,span)*fminf(1,fmaxf(0,pos01+rnd*.2f*gr_noise()));
    g.pos=(float)gr_head-behind;g.pan=.5f+rnd*.45f*gr_noise();g.gain=.75f;}}
  float l=0,r=0;
  for(int j=0;j<GR_MAX;j++){GrainVoice &g=gr[j];if(g.age>=g.life)continue;
   const float w=md_beads_envelope(shape,g.age*g.inc)*g.gain,x=gr_read(g.pos)*w;l+=x*(1-g.pan);r+=x*g.pan;
   g.pos+=g.rate*wob;g.age++;}
  wl[k]=l;wr[k]=r;
  // The tape: the send and the feedback, every step outputs (half rate: CLEAN and DIRTY TAPE, twice as long).
  gr_fb+=((l+r)*.5f-gr_fb)*.5f;
  if(!frozen&&++gr_sub>=step){gr_sub=0;float x=clip(send[k]+gr_fb*fb);if(dirty)x=floorf(x*48+.5f)*(1.0f/48); // coarse steps
   gr_tape[gr_head]=(int16_t)(x*32767);gr_head=(gr_head+1)&GR_M;if(gr_valid<GR_N)gr_valid++;stats.grain_writes++;
   const unsigned col=gr_head>>8;const uint8_t a=(uint8_t)fminf(255,fabsf(x)*254.9f);if(col!=tape_peak_col){tape_peak_col=col;tape_peak[col]=0;}if(a>tape_peak[col])tape_peak[col]=a;}
 }
}
