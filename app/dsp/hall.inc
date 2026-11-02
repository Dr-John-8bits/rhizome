/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the HALL, in place of Monades' room. Included by dreams.cpp before quantum().
 * The Griesinger figure of eight from Dattorro's paper, the family of the classic hall algorithms, with the delay
 * lengths of Mutable Instruments' Rings / Clouds reverb (MIT): a pre-delay, four allpasses that diffuse the input,
 * then a loop of two halves (two allpasses and a delay each), each half fed by the other. In the loop: the decay
 * (from the RT60 asked for and the loop's real length), the treble damping, and a low shelf so the bass lasts BASS x
 * as long. SIZE scales the loop's delays; MOD moves the two long delays (the slow shimmer). VINTAGE runs it all at
 * 22.05 kHz: twice the time from the same memory, a band near 10 kHz, half the computation, like the early digital
 * halls. DECAY at the top is infinite: the loop holds, nothing new enters. One 16-bit buffer, lines laid end to end
 * and read behind a shared write pointer, as Mutable Instruments' FxEngine does. */
enum { HALL_N=32768, HALL_MASK=HALL_N-1, HALL_LINES=11, HALL_PRE=10 };
static const float HALL_OUT=.42f; // its level in the effects return, near the room it replaces
static int16_t hall_buf[HALL_N] POOL;
// ap1-ap4 (input), dap1a dap1b del1 (first half), dap2a dap2b del2 (second half), pre-delay (250 ms at 44.1 kHz)
static const uint16_t hall_len[HALL_LINES]={150,214,319,527,2182,2690,4501,2525,2197,6312,11030};
static uint16_t hall_base[HALL_LINES];
static unsigned hall_wp,hall_tap[HALL_LINES]; // where each line is read this block: its start plus its delay (whole samples)
static float hall_lp1,hall_lp2,hall_lo1,hall_lo2,hall_lfo[2],hall_lfo_phase[2],hall_in_acc,hall_l,hall_r,hall_l_prev,hall_r_prev,hall_env[3],hall_split_lo,hall_split_hi;
static bool hall_odd;
struct HallSet { float krt,bass,klp,kap,size,mod,gain,pre; bool vintage; };
static HallSet hall_set;
static void hall_reset(){
 memset(hall_buf,0,sizeof hall_buf);unsigned b=0;for(int k=0;k<HALL_LINES;k++){hall_base[k]=(uint16_t)b;b+=hall_len[k];}
 hall_wp=0;hall_lp1=hall_lp2=hall_lo1=hall_lo2=hall_in_acc=hall_l=hall_r=hall_l_prev=hall_r_prev=hall_split_lo=hall_split_hi=0;memset(hall_env,0,sizeof hall_env);hall_odd=false;
}
static inline float hall_read(int k,float d){const int i=(int)d;const float f=d-(float)i;const unsigned at=hall_wp+hall_base[k]+(unsigned)i;
 return (hall_buf[at&HALL_MASK]*(1-f)+hall_buf[(at+1)&HALL_MASK]*f)*(1.0f/32768.0f);}
static inline float hall_tap_of(int k){return hall_buf[(hall_wp+hall_tap[k])&HALL_MASK]*(1.0f/32768.0f);}
static inline void hall_write(int k,float x){x=x>1?1:x<-1?-1:x;hall_buf[(hall_wp+hall_base[k])&HALL_MASK]=(int16_t)(x*32767+(x<0?-.5f:.5f));} // rounded: truncation would hasten the quietest tail
static inline float hall_allpass(int k,float x,float g){const float v=hall_tap_of(k),w=x+g*v;hall_write(k,w);return v-g*w;}
// RT60 in seconds for DECAY 0..999 (0.5 s to 20 s, even steps in the logarithm); 1000 is infinite (-1).
static float hall_rt60(int decay){return decay>=1000?-1:.5f*exp2f(decay*(5.321928f/999));}
static float hall_bass_x(int bass){return bass>=500?exp2f((bass-500)*.002f):1/exp2f((500-bass)*.002f);} // 0.5x .. 2x, 500 = 1x
static float hall_rate(){return patch.hall_vintage||fx_shed?22050.0f:44100.0f;}
// The loop's mean length in samples: half of each figure-of-eight turn.
static float hall_loop(float size){return size*.5f*(2182+2690+4501+2525+2197+6312);}
// Loop gain for an RT60: 10^(-3 T / RT60), computed as 1 / 2^(+x) (no negative exponent).
static float hall_gain_for(float rt,float size){return rt<0?1:1/exp2f(9.965784f*hall_loop(size)/hall_rate()/rt);}
static float hall_treble_hz(int treble,bool vintage){const float hz=1500*exp2f(treble*.0036f);return fminf(hz,vintage?9000:16000);} // 1.5 kHz .. 18 kHz
static float hall_onepole(float hz,float rate){const float w=6.2831853f*hz/rate;return w/(1+w);}
// Once a block: the settings, from the patch (and the comet's swell: a longer tail, more input).
static void hall_block(unsigned n){
 HallSet &s=hall_set;s.vintage=patch.hall_vintage||fx_shed;const float rate=hall_rate();
 s.size=.35f+.65f*patch.hall_size*.001f;
 float rt=hall_rt60(patch.reverb_time);if(rt>0)rt*=1+3*comet_swell;
 s.krt=hall_gain_for(rt,s.size);s.bass=rt<0?0:hall_gain_for(rt*hall_bass_x(patch.hall_bass),s.size)/s.krt-1; // the low shelf's extra
 s.klp=rt<0?1:hall_onepole(hall_treble_hz(patch.room_tone,s.vintage),rate);
 s.kap=.45f+.3f*patch.room_diffusion*.001f;s.mod=patch.hall_mod*.001f*(s.vintage?40:80);
 s.gain=rt<0?0:.25f*(1+comet_swell);s.pre=fmaxf(1,patch.hall_predelay*rate*.001f);
 for(int k=0;k<HALL_LINES;k++)hall_tap[k]=hall_base[k]+(unsigned)(k<4?hall_len[k]-2:k==HALL_PRE?s.pre:(hall_len[k]-2)*s.size); // SIZE scales the loop
 for(int k=0;k<2;k++){hall_lfo_phase[k]+=n*(k?.3f:.5f)/MD_RATE;if(hall_lfo_phase[k]>=1)hall_lfo_phase[k]-=1;hall_lfo[k]=sinf(6.2831853f*hall_lfo_phase[k]);}
}
// One step of the loop at the hall's rate.
static void hall_tick(float in){
 const HallSet &s=hall_set;
 hall_wp--;
 hall_write(HALL_PRE,in*s.gain);float x=hall_tap_of(HALL_PRE);
 for(int k=0;k<4;k++)x=hall_allpass(k,x,s.kap);
 float a=x+s.krt*hall_read(9,fminf(6261*s.size+s.mod*hall_lfo[1],6310));
 hall_lp1+=s.klp*(a-hall_lp1);a=hall_lp1;hall_lo1+=.03f*(a-hall_lo1);a+=s.bass*hall_lo1;
 a=hall_allpass(4,a,-s.kap);a=hall_allpass(5,a,s.kap);hall_write(6,a);hall_l=2*a;
 float b=x+s.krt*hall_read(6,fminf(4460*s.size+s.mod*hall_lfo[0],4499));
 hall_lp2+=s.klp*(b-hall_lp2);b=hall_lp2;hall_lo2+=.03f*(b-hall_lo2);b+=s.bass*hall_lo2;
 b=hall_allpass(7,b,s.kap);b=hall_allpass(8,b,-s.kap);hall_write(9,b);hall_r=2*b;
}
// One output sample; VINTAGE steps the loop on every other one (inputs averaged, outputs joined by a line).
static void hall_sample(float in,float *left,float *right){
 if(!hall_set.vintage){hall_tick(in);*left=hall_l;*right=hall_r;}
 else{hall_in_acc+=in*.5f;hall_odd=!hall_odd;
  if(!hall_odd){hall_l_prev=hall_l;hall_r_prev=hall_r;hall_tick(hall_in_acc);hall_in_acc=0;*left=hall_l;*right=hall_r;}
  else{*left=.5f*(hall_l+hall_l_prev);*right=.5f*(hall_r+hall_r_prev);}}
 // The tail's energy in three bands (below 250 Hz, middle, above 5 kHz), for the screen.
 const float m=.5f*(*left+*right);hall_split_lo+=.035f*(m-hall_split_lo);hall_split_hi+=.5f*(m-hall_split_hi);
 const float band[3]={hall_split_lo,hall_split_hi-hall_split_lo,m-hall_split_hi};
 for(int k=0;k<3;k++)hall_env[k]+=(band[k]*band[k]-hall_env[k])*.0005f;
}
// For the screen: how long the low, middle and high parts of the tail take to fall 60 dB (-1: never).
static void hall_rt60s(float out[3]){
 const HallSet &s=hall_set;const float rt=hall_rt60(patch.reverb_time);
 if(rt<0){out[0]=out[1]=out[2]=-1;return;}
 out[0]=rt*hall_bass_x(patch.hall_bass);out[1]=rt;
 const float w=6.2831853f*6000/hall_rate(),c=s.klp,q=1-c,mag=c/sqrtf(1-2*q*cosf(w)+q*q),g=s.krt*mag; // the damping at 6 kHz
 out[2]=g>=1?rt:3*hall_loop(s.size)/hall_rate()/(log2f(1/g)*.30103f);
}
