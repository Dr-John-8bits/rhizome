/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the orbital sequencer. Included by dreams.cpp, inside its anonymous namespace, after fire().
 * A track with gates is a body on an ellipse around a star. Its mean anomaly follows the sequencer's steps, so the orbit
 * keeps time with the tempo, the swing and a received clock, and PLAY starts every body from the top. Kepler's equation
 * turns the mean anomaly into a position; the body plays when it crosses one of its gates, fixed directions around the
 * star spread as a euclidean rhythm. A round orbit plays that rhythm exactly. An eccentric one hurries near the star and
 * lingers far from it (Kepler's second law): the same loop, with rubato. Precession turns each ellipse slowly, so the
 * rubato slides against the bar; two bodies aligned with the star (a conjunction) accent their next beat. Each note
 * spins the body's moons once around it; a moon plays an ornament as it passes the body's outer side. */
static const int16_t orbit_steps_of[MD_ORBIT_PERIODS]={4,6,8,12,16,24,32,48,64,96,112,128,192};
static const int8_t orbit_moon_steps_of[MD_MOON_SPINS]={1,2,3,4,6,8,12,16};
static const float ORBIT_TAU=6.28318531f,ORBIT_PI=3.14159265f;
static const int16_t orbit_comet_steps_of[MD_COMETS]={0,64,128,256,512,1024};
static const float COMET_E=MD_COMET_ECC_MILLI*.001f;
static float comet_M0=ORBIT_PI,comet_target,comet_swell; // its mean anomaly at PLAY; what its passage asks for now, followed
static const float orbit_tilt[6]={-.6f,.5f,1.6f,2.7f,3.8f,4.9f}; // each periapsis in its own direction (radians)
struct Orbit { float M0,w0,gate_M[MD_ORBIT_GATES_MAX];int gate_n,motif;float accent_until,moon_t0,moon_vel; };
static Orbit orbits[6];
static float orbit_step_len=5512.5f,orbit_since,orbit_prev=-1; // samples in the current step, since its tick; last position run
static unsigned orbit_rng=1,orbit_rest; // its own random stream (perturbation); tracks FORM rests this step
static int orbit_prepared=-1;
static float conj_d[15],conj_cool[15];static uint8_t conj_seen[15];

// No atan on the FM-1: a minimax polynomial on [-1, 1] (error under 1e-5 rad), folded for the rest.
static float orbit_atan(float x){
 const bool fold=fabsf(x)>1;const float z=fold?1/x:x,z2=z*z;
 const float a=z*(.9998660f+z2*(-.3302995f+z2*(.1801410f+z2*(-.0851330f+z2*.0208351f))));
 return fold?(x>0?1.57079633f:-1.57079633f)-a:a;
}
static float orbit_atan2(float y,float x){
 if(x>0)return orbit_atan(y/x);
 if(x<0)return orbit_atan(y/x)+(y>=0?ORBIT_PI:-ORBIT_PI);
 return y>0?1.57079633f:y<0?-1.57079633f:0;
}
static float orbit_wrap(float x){return x-ORBIT_TAU*floorf(x/ORBIT_TAU);}
static float orbit_E(float M,float e){ // Kepler's equation by Newton's method
 M=orbit_wrap(M);float E=e<.8f?M:ORBIT_PI;
 for(int k=0;k<6;k++)E-=(E-e*sinf(E)-M)/(1-e*cosf(E));
 return E;
}
static float orbit_true_of(float E,float e){return 2*orbit_atan2(sqrtf(1+e)*sinf(E*.5f),sqrtf(1-e)*cosf(E*.5f));}
static float orbit_mean_of(float theta,float e){
 const float E=2*orbit_atan2(sqrtf(1-e)*sinf(theta*.5f),sqrtf(1+e)*cosf(theta*.5f));
 return E-e*sinf(E);
}
static int orbit_steps(unsigned i){return orbit_steps_of[bound(patch.orbit[i].period,0,MD_ORBIT_PERIODS-1)];}
// Eccentricity now: the perturbation makes it breathe slowly (a round orbit stays round).
static float orbit_e(unsigned i,float pos){
 float e=patch.orbit[i].ecc*.001f;
 if(e>.02f&&patch.orbit_pert)e+=patch.orbit_pert*.0008f*sinf(ORBIT_TAU*pos/(orbit_steps(i)*7.3f)+i*1.7f);
 return e<0?0:e>.9f?.9f:e;
}
static float orbit_rate(){return patch.orbit_prec*(ORBIT_TAU/(100.0f*256.0f));} // radians a step: 100 = a turn in 16 bars
static float orbit_w(unsigned i,float pos){return orbits[i].w0+orbit_rate()*pos;}
static float orbit_M(unsigned i,float pos){return orbits[i].M0+ORBIT_TAU*pos/orbit_steps(i);}
// Where the sequence is, in steps since PLAY (fractional between ticks); -1 before the first step.
static float orbit_pos_now(){return step?(float)(step-1)+fminf(orbit_since/orbit_step_len,.999f):-1.0f;}

static void orbit_reset(){ // PLAY: every body starts at the top, together
 orbit_prev=-1;orbit_since=0;orbit_prepared=-1;orbit_rng=patch.seed|1;orbit_rest=0;
 for(unsigned i=0;i<6;i++){Orbit &o=orbits[i];o.w0=orbit_tilt[i];o.M0=orbit_mean_of(-1.57079633f-o.w0,orbit_e(i,0));o.accent_until=-1;o.motif=0;o.gate_n=0;o.moon_t0=-100;
  stats.moon_t0[i]=-100;stats.moon_hit[i]=0;stats.orbit_hit_gate[i]=-1;stats.orbit_hit_pos[i]=-100;stats.orbit_hit_skip[i]=0;}
 comet_M0=ORBIT_PI;stats.comet_M=ORBIT_PI;comet_target=0; // the comet starts far away
 memset(conj_cool,0,sizeof conj_cool);memset(conj_seen,0,sizeof conj_seen);stats.conj_a=stats.conj_b=-1;stats.conj_pos=-100;stats.orbit_pos=0;
}
// A new period or precession keeps each body where it is (no jump, no burst of gates).
static void orbit_rebase_period(unsigned i,int period){
 const float pos=orbit_pos_now()<0?0:orbit_pos_now(),M=orbit_M(i,pos);
 patch.orbit[i].period=period;orbits[i].M0=M-ORBIT_TAU*pos/orbit_steps(i);orbit_prepared=-1;
}
static float orbit_comet_M(float pos){const int n=orbit_comet_steps_of[bound(patch.orbit_comet,0,MD_COMETS-1)];return n?comet_M0+ORBIT_TAU*pos/n:comet_M0;}
static void orbit_rebase_comet(int comet){
 const float pos=orbit_pos_now()<0?0:orbit_pos_now(),M=orbit_comet_M(pos);patch.orbit_comet=comet;
 const int n=orbit_comet_steps_of[comet];comet_M0=n?M-ORBIT_TAU*pos/n:M;
}
// How strongly the comet acts: nothing beyond its mean distance, all of it at the star (smooth in between): about a
// fifth of its turn.
static float orbit_comet_strength(float M){const float r=1-COMET_E*cosf(orbit_E(M,COMET_E)),x=(1-r)/COMET_E;return x<=0?0:x>=1?1:x*x*(3-2*x);}
// Once a block: the room swell follows the passage (about 70 ms), and stops gently with the sequence.
static void orbit_comet_follow(){comet_swell+=((playing&&patch.orbit_comet&&patch.comet_event!=MD_COMET_FREEZE?comet_target:0)-comet_swell)*.02f;}
static bool orbit_comet_frozen(){return playing&&patch.orbit_comet&&patch.comet_event!=MD_COMET_SWELL&&comet_target>.35f;}
static void orbit_rebase_prec(int prec){
 const float pos=orbit_pos_now()<0?0:orbit_pos_now();float w[6];
 for(unsigned i=0;i<6;i++)w[i]=orbit_w(i,pos);
 patch.orbit_prec=prec;for(unsigned i=0;i<6;i++)orbits[i].w0=w[i]-orbit_rate()*pos;orbit_prepared=-1;
}
// Each gate as the mean anomaly where the body crosses it, for the current ellipse (once a step: it turns slowly).
static void orbit_prepare(float pos){
 for(unsigned i=0;i<6;i++){Orbit &o=orbits[i];o.gate_n=0;if(!patch.orbit[i].gates)continue;
  const float e=orbit_e(i,pos),w=orbit_w(i,pos);float phi[MD_ORBIT_GATES_MAX];const int n=md_orbit_gate_angles(&patch,(int)i,phi);
  for(int g=0;g<n;g++)o.gate_M[g]=orbit_wrap(orbit_mean_of(phi[g]-w,e));
  o.gate_n=n;}
}
// Tintinnabuli: the nearest note of the tonic triad above.
static float orbit_triad_above(float src){
 const float r=(float)(48+patch.root),third=scale_neighbour(scale_neighbour(r,1),1),fifth=scale_neighbour(scale_neighbour(third,1),1);
 const int pcs[3]={patch.root,(int)floorf(third+.5f)%12,(int)floorf(fifth+.5f)%12};
 for(int k=1;k<=12;k++){const int n=(int)floorf(src+.5f)+k;for(int j=0;j<3;j++)if(n%12==pcs[j])return (float)n;}
 return src+12;
}
// A conjunction as a chord: both bodies strike now. The second takes the note of the first's triad (a third, fifth or
// octave in the scale) nearest its own, so each keeps its register.
static void orbit_chord(unsigned a,unsigned b){
 const float pa=stats.last_pitch[a];
 const unsigned pair[2]={a,b};for(unsigned i:pair){const md_track &t=patch.track[i];float p=stats.last_pitch[i];if(t.mute||((orbit_rest>>i)&1)||p<=0)continue;
  if(i==b&&t.model>=MD_FM&&patch.track[a].model>=MD_FM&&pa>0){float best=p,d=99;
   for(int deg=0;deg<=4;deg+=2){float c=pa;for(int s=0;s<deg;s++)c=scale_neighbour(c,1);c+=12*floorf((p-c)/12+.5f);if(fabsf(c-p)<d){d=fabsf(c-p);best=c;}}
   p=best;}
  fire(i,p,.8f);out_note(i,p,.8f,orbit_step_len*2);}
}
static void orbit_fire(unsigned i,int gate,float pos){
 const md_track &t=patch.track[i];const md_orbit &c=patch.orbit[i];Orbit &o=orbits[i];
 stats.orbit_hit_gate[i]=gate;stats.orbit_hit_pos[i]=pos;stats.orbit_hit_skip[i]=1;
 if(t.mute||((orbit_rest>>i)&1))return;
 const unsigned r=random_word(orbit_rng);const float pert=patch.orbit_pert*.01f;
 if(r%1000<(unsigned)(pert*250))return; // the perturbation refuses a quarter of the gates at most
 const float e=orbit_e(i,pos),E=orbit_E(orbit_M(i,pos),e),rn=e<.01f?.5f:.5f-.5f*cosf(E); // 0 at the star, 1 farthest
 float pitch=(float)t.note;
 if(t.model>=MD_FM){
  if(c.pitch==MD_PITCH_MOTIF){const int32_t *ph=patch.ext[i].notes;int found=-1;
   for(int k=0;k<16&&found<0;k++){const int at=(o.motif+k)%16;if(ph[at]>=0)found=at;}
   if(found<0)return;o.motif=found+1;pitch+=ph[found];}
  else if(c.pitch==MD_PITCH_ECHO){const float src=stats.last_pitch[(i+5)%6];if(src<=0)return;pitch=orbit_triad_above(src);}
  else{int deg=(int)floorf((1-rn)*6+.5f); // near the star: high
   if((r>>10)%1000<(unsigned)(pert*500))deg+=(r>>20)&1?1:-1;
   pitch=quantize_any(pitch,patch.root,patch.scale,0);
   for(int k=0;k<deg;k++)pitch=scale_neighbour(pitch,1);for(int k=0;k>deg;k--)pitch=scale_neighbour(pitch,-1);}
  pitch=quantize_any(pitch+patch.transpose,patch.root,patch.scale,0);
 }
 const float velocity=o.accent_until>pos?1.0f:.55f+.15f*(1-rn);
 stats.orbit_hit_skip[i]=0;stats.orbit_hits[i]++;
 fire(i,pitch,velocity);out_note(i,pitch,velocity,orbit_step_len*.5f);
 if(c.moons){o.moon_t0=stats.moon_t0[i]=pos;o.moon_vel=velocity; // the note spins its moons; each one's ornament, from it
  static const int8_t harmonics[MD_MOONS_MAX]={12,19,24};
  for(int k=1;k<=c.moons;k++){float m=pitch;
   if(t.model>=MD_FM){const int deg=c.moon_interval==MD_MOON_UP?k:c.moon_interval==MD_MOON_DOWN?-k:c.moon_interval==MD_MOON_THIRDS_UP?2*k:c.moon_interval==MD_MOON_THIRDS_DOWN?-2*k:0;
    if(c.moon_interval==MD_MOON_HARMONICS)m=quantize_any(m+harmonics[k-1],patch.root,patch.scale,0);
    for(int s=0;s<deg;s++)m=scale_neighbour(m,1);for(int s=0;s>deg;s--)m=scale_neighbour(m,-1);}
   stats.moon_note[i][k-1]=m;}}
}
static float orbit_moon_steps(unsigned i){return orbit_moon_steps_of[bound(patch.orbit[i].moon_spin,0,MD_MOON_SPINS-1)];}
// Moon k's ornament, quieter than the note it follows.
static void orbit_moon_fire(unsigned i,int k){
 const md_orbit &c=patch.orbit[i];const Orbit &o=orbits[i];
 stats.moon_hit[i]=k;stats.moon_hits[i]++;
 if(patch.track[i].mute||((orbit_rest>>i)&1))return;
 const float pitch=stats.moon_note[i][k-1],velocity=o.moon_vel*c.moon_level*.01f*(1-.15f*(k-1));if(velocity<=0)return;
 // An ornament, not a new note: ECHO and the next meend still follow the body's own note.
 const float last=stats.last_pitch[i];const int32_t last_note=stats.last_note[i];
 fire(i,pitch,velocity);out_note(i,pitch,velocity,orbit_step_len*orbit_moon_steps(i)/(c.moons+1)*.6f);
 stats.last_pitch[i]=last;stats.last_note[i]=last_note;
}
// Once per rendered block, before the voices: gates crossed since the last block, then conjunctions.
static void orbit_run(){
 const float pos=orbit_pos_now();if(pos<0)return;
 if(pos<orbit_prev||pos-orbit_prev>4)orbit_prev=pos-1e-4f; // a restart or a long gap: no burst
 if((int)step!=orbit_prepared){orbit_prepare(pos);orbit_prepared=(int)step;}
 float phi[6];bool on[6];
 for(unsigned i=0;i<6;i++){const Orbit &o=orbits[i];const float M1=orbit_M(i,orbit_prev),M2=orbit_M(i,pos),e=orbit_e(i,pos),w=orbit_w(i,pos);
  for(int g=0;g<o.gate_n;g++){float m=o.gate_M[g]+ORBIT_TAU*ceilf((M1-o.gate_M[g])/ORBIT_TAU);if(m<=M1)m+=ORBIT_TAU;if(m<=M2)orbit_fire(i,g,pos);}
  // Moons: moon k passes the outer side k/(n+1) of the way through its spin.
  if(const int n=patch.orbit[i].moons){const float span=orbit_moon_steps(i),f1=(orbit_prev-o.moon_t0)/span*(n+1),f2=(pos-o.moon_t0)/span*(n+1);
   if(o.moon_t0>-100&&f1<n)for(int k=1;k<=n;k++)if(f1<k&&k<=f2)orbit_moon_fire(i,k);}
  stats.orbit_M[i]=M2;stats.orbit_w[i]=w;stats.orbit_e[i]=e;
  on[i]=patch.orbit[i].gates&&!patch.track[i].mute;if(on[i])phi[i]=orbit_true_of(orbit_E(M2,e),e)+w;}
 for(unsigned i=0,k=0;i<6;i++)for(unsigned j=i+1;j<6;j++,k++){
  if(!on[i]||!on[j]){conj_seen[k]=0;continue;}
  float d=phi[i]-phi[j];d-=ORBIT_TAU*floorf((d+ORBIT_PI)/ORBIT_TAU);
  if(conj_seen[k]&&(d<0)!=(conj_d[k]<0)&&fabsf(d)<.5f&&fabsf(conj_d[k])<.5f&&pos>=conj_cool[k]){
   conj_cool[k]=pos+8;
   if(patch.orbit_conj==MD_CONJ_ACCENT)orbits[i].accent_until=orbits[j].accent_until=pos+4; // their next beat sings out
   else if(patch.orbit_conj==MD_CONJ_CHORD)orbit_chord(i,j);
   stats.conj_a=(int32_t)i;stats.conj_b=(int32_t)j;stats.conj_pos=pos;stats.conj_count++;}
  conj_d[k]=d;conj_seen[k]=1;}
 // The comet: a passage each time its mean anomaly comes round to the star.
 if(patch.orbit_comet){const float M1=orbit_comet_M(orbit_prev),M2=orbit_comet_M(pos);
  if(floorf(M2/ORBIT_TAU)>floorf(M1/ORBIT_TAU))stats.comet_passes++;
  stats.comet_M=orbit_wrap(M2);comet_target=orbit_comet_strength(M2)*patch.comet_amount*.01f;}
 else comet_target=0;
 stats.comet_now=comet_target;stats.orbit_pos=pos;orbit_prev=pos;
}
