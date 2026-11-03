/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the tape echo, in place of Monades' echo (same 32 KiB). Included by dreams.cpp before quantum().
 * A loop of tape (32768 samples, 8-bit mu-law) runs under a record head and three playback heads at fixed distances:
 * a third, two thirds and all of the path. TIME sets the speed so that the longest head in use repeats after TIME:
 * fast tape for short echoes (clean), slow tape for long ones (darker, up to about 2.9 s), as on a real machine. The
 * speed has inertia, so a new time slides the pitch; wow, flutter and drift wobble it. The record head saturates
 * (DRIVE); each pass goes through the tone filter, darker as the tape slows; AGE adds hiss, loses highs and drops out.
 * FREEZE lifts the record head: the loop plays what is on it, again and again. STOP brakes the tape: the echoes dive
 * and fall silent; RUN starts it again. */
enum { REEL_N=32768, REEL_MASK=REEL_N-1 };
static const float REEL_PATH=31500,REEL_SPEED_MIN=.5f,REEL_SPEED_MAX=12; // tape samples, record head to head 3
static uint8_t reel[REEL_N] POOL;
static int16_t reel_decode[256];
static float reel_pos,reel_speed=1,reel_mod=1,reel_wow_phase,reel_flutter_phase,reel_drift,reel_lp,reel_tone_lp,reel_hp,
 reel_drop=1,reel_drop_target=1,reel_head_peak[3],reel_rec_peak,reel_wow_now,reel_env;
static unsigned reel_rng=0x5eed1234u,reel_drop_left,reel_trace_col;
static int reel_transport;
static bool reel_snap; // after a reset the empty tape starts at its speed, without a slide
static uint8_t reel_trace[128]; // the level written on each 256-sample stretch of the loop, for the screen
static uint8_t reel_encode(float x){
 int s=(int)(x*32767);const int sign=s<0?0x80:0;if(s<0)s=-s;if(s>32635)s=32635;s+=132;
 const int top=31-__builtin_clz((unsigned)s),e=top>7?top-7:0; // the exponent: where the highest bit is
 return (uint8_t)~(sign|(e<<4)|((s>>(e+3))&0x0F));
}
static void reel_reset(){
 for(int u=0;u<256;u++){const int x=~u&0xFF;int t=((x&0x0F)<<3)+132;t<<=(x&0x70)>>4;reel_decode[u]=(int16_t)((x&0x80)?132-t:t-132);}
 memset(reel,reel_encode(0),sizeof reel);memset(reel_trace,0,sizeof reel_trace);memset(reel_head_peak,0,sizeof reel_head_peak);
 reel_rng=0x5eed1234u;reel_wow_phase=reel_flutter_phase=0;reel_mod=1;reel_snap=true; // the same Monde always sounds the same
 reel_pos=0;reel_env=0;reel_lp=reel_tone_lp=reel_hp=reel_rec_peak=reel_wow_now=reel_drift=0;reel_drop=reel_drop_target=1;reel_drop_left=0;
}
static int reel_heads(){const int h=patch.tape_heads&7;return h?h:4;}
static float reel_longest(){const int h=reel_heads();return REEL_PATH*(h&4?3:h&2?2:1)/3;}
// The times the tape can give for the heads in use, at its slowest and fastest.
static float reel_ms_max(){return reel_longest()/(REEL_SPEED_MIN*22.05f);}
static float reel_ms_min(){return reel_longest()/(REEL_SPEED_MAX*22.05f);}
static float reel_noise(){reel_rng=reel_rng*1664525u+1013904223u;return (float)(int32_t)reel_rng*(1.0f/2147483648.0f);}
static float reel_at_linear(float p){ // fast tape: the heads skip samples anyway
 const int i=(int)(p+REEL_N)-REEL_N;const float f=p-(float)i; // p > -REEL_N: no floorf
 return (reel_decode[reel[i&REEL_MASK]]*(1-f)+reel_decode[reel[(i+1)&REEL_MASK]]*f)*(1.0f/32768.0f);
}
static float reel_at(float p){ // Hermite between tape samples
 const int i=(int)(p+REEL_N)-REEL_N;const float f=p-(float)i; // p > -REEL_N: no floorf
 const float xm=reel_decode[reel[(i-1)&REEL_MASK]],x0=reel_decode[reel[i&REEL_MASK]],x1=reel_decode[reel[(i+1)&REEL_MASK]],x2=reel_decode[reel[(i+2)&REEL_MASK]];
 const float c=(x1-xm)*.5f,v=x0-x1,w=c+v,a=w+v+(x2-x0)*.5f,b=w+a;
 return ((((a*f)-b)*f+c)*f+x0)*(1.0f/32768.0f);
}
// Once a block: the speed (inertia toward TIME, braking for STOP), wow, flutter, drift, dropouts.
static float reel_rec_coef,reel_age;
static void reel_block(unsigned n,float time_ms,float glide){
 const float run=fminf(REEL_SPEED_MAX,fmaxf(REEL_SPEED_MIN,reel_longest()/(time_ms*22.05f)));
 const float target=reel_transport==MD_TAPE_STOP?0:reel_transport==MD_TAPE_REWIND?-fminf(REEL_SPEED_MAX,3*run):run; // REWIND: back, three times as fast
 const float g=reel_transport==MD_TAPE_STOP||reel_transport==MD_TAPE_REWIND||reel_speed<REEL_SPEED_MIN*.9f?.00025f:glide; // the motor turns round slowly
 if(reel_snap){reel_snap=false;reel_speed=target;}
 for(unsigned k=0;k<n;k++)reel_speed+=(target-reel_speed)*g; // per sample, as before: the same slide
 const float wow=patch.tape_wow*.001f;
 reel_wow_phase+=n*(.55f/MD_RATE);if(reel_wow_phase>=1)reel_wow_phase-=1;reel_flutter_phase+=n*(7.3f/MD_RATE);if(reel_flutter_phase>=1)reel_flutter_phase-=1;
 reel_drift+=.02f*(reel_noise()-reel_drift);
 reel_wow_now=.6f*sinf(6.2831853f*reel_wow_phase)+.2f*sinf(6.2831853f*reel_flutter_phase)+.2f*reel_drift*5; // about -1..1
 reel_mod=(1+wow*.01f*reel_wow_now)*(1-.3f*shock_burst); // a WANDER shock: the tape stumbles (orbit.inc)
 // Slower tape holds fewer highs: the record head's filter follows the tape rate (and AGE takes more).
 reel_age=patch.tape_age*.001f;
 const float rate=22050*fmaxf(fabsf(reel_speed),.05f),fc=fminf(.45f,.42f*rate/MD_RATE)*(1-.55f*reel_age);const float w=6.2831853f*fc;reel_rec_coef=w/(1+w); // a one-pole at fc (close to 1-exp(-w), without exp)
 if(reel_drop_left){reel_drop_left=reel_drop_left>n?reel_drop_left-n:0;if(!reel_drop_left)reel_drop_target=1;}
 else if(reel_age>0&&(reel_rng>>8)%100000<(unsigned)(reel_age*reel_age*60)){reel_drop_target=.15f+.3f*fabsf(reel_noise());reel_drop_left=1300+(reel_rng>>20)%2600;}
}
// One sample: what the heads read now (left, right), and the send recorded with the feedback.
static void reel_sample(float in,float feedback,float tone,float *left,float *right){
 const int h=reel_heads(),n=(h&1)+((h>>1)&1)+((h>>2)&1);
 static const float pans[3]={.3f,.5f,.7f};
 float sum=0,l=0,r=0;const float gain=n==1?1:n==2?.75f:.6f,live=fminf(1,fabsf(reel_speed)*3);
 reel_drop+=(reel_drop_target-reel_drop)*.004f;
 for(int k=0;k<3;k++)if(h>>k&1){
  const float at=reel_pos-REEL_PATH*(k+1)/3,y=(fabsf(reel_speed)>2?reel_at_linear(at):reel_at(at))*live*reel_drop;sum+=y;
  const float pan=n==1?.5f:pans[k];l+=y*(1-pan)*gain;r+=y*pan*gain;
  if(fabsf(y)>reel_head_peak[k])reel_head_peak[k]=fabsf(y);}
 *left=l;*right=r;
 // The feedback path: the heads, darker each pass (TONE), without the lowest rumble.
 reel_tone_lp+=(sum/n-reel_tone_lp)*tone*(1-.5f*reel_age);reel_hp+=.003f*(reel_tone_lp-reel_hp);
 const float drive=1+4*patch.tape_drive*.001f;
 float x=(in+feedback*(reel_tone_lp-reel_hp))*drive;
 x=x*(27+x*x)/(27+9*x*x);x=fminf(1,fmaxf(-1,x))/(.7f+.3f*drive); // the record head saturates
 const float level=fabsf(x);reel_env+=(level-reel_env)*(level>reel_env?.01f:.0002f);
 if(reel_env>1e-6f)x+=reel_noise()*(.0004f+.004f*reel_age)*fminf(1,reel_env*20); // hiss, under what is recorded (none on an empty tape)
 reel_lp+=(x-reel_lp)*reel_rec_coef;
 // The tape moves on; each tape sample the record head passes is written (unless FREEZE or REWIND has lifted it).
 const float from=reel_pos,to=reel_pos+.5f*reel_speed*reel_mod;
 const int first=(int)from+1,last=(int)to; // positions are never negative
 if(reel_transport!=MD_TAPE_FREEZE&&reel_transport!=MD_TAPE_REWIND&&last>=first){ // one encoding for the samples passed in this one (fast tape)
  const uint8_t code=reel_encode(reel_lp);
  for(int p=first;p<=last;p++)reel[p&REEL_MASK]=code;
  const unsigned col=((unsigned)last&REEL_MASK)>>8;const float v=fabsf(reel_lp);const uint8_t a=(uint8_t)fminf(255,v*600);
  if(col!=reel_trace_col){reel_trace_col=col;reel_trace[col]=0;}if(a>reel_trace[col])reel_trace[col]=a;if(v>reel_rec_peak)reel_rec_peak=v;}
 reel_pos=to>=REEL_N?to-REEL_N:to<0?to+REEL_N:to; // REWIND runs it backwards
}
