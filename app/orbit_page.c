/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the ORBIT, ORBIT / SYSTEM, ORBIT / MOONS and ORBIT / COMET pages. Included by app.c before md_draw_all.
 * One large drawing, a thin strip for K1-K4, and the card of the knob being turned in the corner opposite what it
 * changes. Everything drawn is the engine's state: the ellipses as they are now (precession, breathing), the gates
 * where the bodies play, each body where it is with a trail whose length is its speed (the rubato), a ripple for each
 * note, the gate lit when it plays (grey when the perturbation refused it), a line for a conjunction, the bar on the
 * outer ring, the moons where the last note's spin has taken them (each lit as it plays, on the body's outer side).
 * The comet where it is, its tail pointing away from the star and as long as its passage acts; beyond the bar ring,
 * a mark on the ring in its direction. The geometry is computed once a frame (orbit_view_update), then drawn band by band. */
enum { OV_CX=120, OV_CY=119, OV_RING=78, OV_TRAIL=10, OV_MOON=9 };
typedef struct { int on; float a,e,w,x,y,tx[OV_TRAIL],ty[OV_TRAIL],out,mx[MD_MOONS_MAX],my[MD_MOONS_MAX]; } OrbitView;
static OrbitView ov[6],ov_comet; // the comet: a fixed ellipse, periapsis 8 pixels from the star, down and to the left
// Where the system is drawn: the ORBIT pages, or smaller on the home page and the start-up (orbit_frame). ov_fit is the
// outermost reach in pixels; ov_top and ov_bottom keep the ellipses off the title and the strip; ov_demo, in sixteenths,
// moves the stopped bodies along their real orbits at their real speeds (the start-up, before anything plays).
static float ov_cx=OV_CX,ov_cy=OV_CY,ov_fit=72,ov_top=42,ov_bottom=196,ov_demo;
static void orbit_frame(float cx,float cy,float fit,float top,float bottom,float demo){ov_cx=cx;ov_cy=cy;ov_fit=fit;ov_top=top;ov_bottom=bottom;ov_demo=demo;}
static float ov_pos,ov_level;
static uint32_t ov_hits_seen[6],ov_hit_ms[6],ov_conj_seen,ov_conj_ms,ov_touch_ms,ov_moon_seen[6],ov_moon_ms[6];
static int ov_moon_k[6];
static int ov_hit_gate[6]={-1,-1,-1,-1,-1,-1},ov_hit_skip[6],ov_conj_a=-1,ov_conj_b=-1,ov_touch=-1,ov_touch_page=-1;
static float ov_hit_x[6],ov_hit_y[6],ov_hit_vel[6];

static void orbit_touch(int knob){ov_touch=knob;ov_touch_page=dream_ui.page;ov_touch_ms=plat_ms();}
// Where a body is, from its mean anomaly (Kepler's equation), on the screen: x right, y down, the star at the focus.
static void ov_point(const OrbitView *o,float M,float *x,float *y){
 const float E=md_orbit_eccentric(M,o->e),ex=o->a*(fm_cosf(E)-o->e),ey=o->a*fm_sqrtf(1-o->e*o->e)*fm_sinf(E);
 const float c=fm_cosf(o->w),s=fm_sinf(o->w);*x=ov_cx+ex*c-ey*s;*y=ov_cy+ex*s+ey*c;
}
// The mean anomaly where a body sits at direction phi (stopped: all at the top, where PLAY starts them).
static float ov_mean_at(float phi,float w,float e){const float t=(phi-w)*.5f,E=2*md_atan2(fm_sqrtf(1-e)*fm_sinf(t),fm_sqrtf(1+e)*fm_cosf(t));return E-e*fm_sinf(E);}
static void orbit_view_update(void){
 const md_patch *p=md_state();const md_stats *ms=md_status();float p23[6],lo=1e9f,hi=0,ext=1;
 ov_pos=ms->orbit_pos;
 // Farther out = slower (Kepler's third law, a ~ T^(2/3)), squeezed between 24 and 62 pixels, then fitted to the ring.
 for(int i=0;i<6;i++){p23[i]=fm_powf((float)md_orbit_steps(p->orbit[i].period),.6666667f);if(p->orbit[i].gates){lo=fm_minf(lo,p23[i]);hi=fm_maxf(hi,p23[i]);}}
 for(int i=0;i<6;i++){OrbitView *o=&ov[i];o->on=p->orbit[i].gates>0||i==dream_ui.track;
  o->a=hi-lo<1e-3f?46:24+38*(p23[i]-lo)/(hi-lo);if(o->a<24)o->a=24;if(o->a>62)o->a=62;
  o->e=md_playing()?ms->orbit_e[i]:p->orbit[i].ecc*.001f;o->w=ms->orbit_w[i];
  if(o->on&&o->a*(1+o->e)>ext)ext=o->a*(1+o->e);}
 const float fit=fm_minf(1.2f*ov_fit/72,ov_fit/ext),moon=OV_MOON*ov_fit/72;
 for(int i=0;i<6;i++){OrbitView *o=&ov[i];o->a*=fit;if(!o->on)continue;
  const float M=md_playing()?ms->orbit_M[i]:ov_mean_at(-1.5707963f,o->w,o->e)+6.2831853f*ov_demo/md_orbit_steps(p->orbit[i].period);ov_point(o,M,&o->x,&o->y);
  for(int k=0;k<OV_TRAIL;k++)ov_point(o,M-(k+1)*(6.2831853f/9/OV_TRAIL),&o->tx[k],&o->ty[k]); // a ninth of a turn behind
  if(ms->orbit_hits[i]!=ov_hits_seen[i]||ms->orbit_hit_gate[i]!=ov_hit_gate[i]){ // a note (or a refused gate) since the last frame
   if(ms->orbit_hits[i]!=ov_hits_seen[i]){ov_hit_ms[i]=plat_ms();ov_hit_x[i]=o->x;ov_hit_y[i]=o->y;ov_hit_vel[i]=ms->track_peak[i];}
   ov_hits_seen[i]=ms->orbit_hits[i];ov_hit_gate[i]=ms->orbit_hit_gate[i];ov_hit_skip[i]=ms->orbit_hit_skip[i];if(ov_hit_skip[i])ov_hit_ms[i]=plat_ms();}
  // Moons: parked behind the outer side, evenly; a note spins them once round, so moon k passes it k/(n+1) of the way.
  {const int n=p->orbit[i].moons;o->out=md_atan2(o->y-ov_cy,o->x-ov_cx);float f=0;
   if(md_playing()&&ms->moon_t0[i]>-100){f=(ms->orbit_pos-ms->moon_t0[i])/md_orbit_moon_steps(p->orbit[i].moon_spin);f=f<0?0:f>1?1:f;}
   for(int k=0;k<n;k++){const float a=o->out+6.2831853f*(f-(k+1)/(float)(n+1));o->mx[k]=o->x+moon*fm_cosf(a);o->my[k]=o->y+moon*fm_sinf(a);}
   if(ms->moon_hits[i]!=ov_moon_seen[i]){ov_moon_seen[i]=ms->moon_hits[i];ov_moon_ms[i]=plat_ms();ov_moon_k[i]=ms->moon_hit[i];}}}
 {OrbitView *c=&ov_comet;c->on=p->orbit_comet>0;c->e=MD_COMET_ECC_MILLI*.001f;c->a=8*ov_fit/72/(1-c->e);c->w=2.3561945f;
  ov_point(c,ms->comet_M,&c->x,&c->y);c->out=md_atan2(c->y-ov_cy,c->x-ov_cx);}
 if(ms->conj_count!=ov_conj_seen){ov_conj_seen=ms->conj_count;ov_conj_ms=plat_ms();ov_conj_a=ms->conj_a;ov_conj_b=ms->conj_b;}
 ov_level=fm_maxf(ms->out_peak[0],ms->out_peak[1]);
}
static int ov_band(float y0,float y1){const float top=-(float)cv_oy;return y1>=top-2&&y0<top+42;} // does [y0, y1] touch this band
static void ov_ring(float cx,float cy,float r,uint16_t ink,float step){
 if(r<=0||!ov_band(cy-r,cy+r))return;
 const int n=(int)(6.2831853f*r/step)+8;for(int k=0;k<n;k++){const float t=k*6.2831853f/n;md_soft_point(cx+r*fm_cosf(t),cy+r*fm_sinf(t),ink);}
}
static void ov_seg(float x0,float y0,float x1,float y1,uint16_t ink){ // an antialiased line, as dots a pixel apart
 if(!ov_band(fm_minf(y0,y1),fm_maxf(y0,y1)))return;
 const float dx=x1-x0,dy=y1-y0,len=fm_sqrtf(dx*dx+dy*dy);const int n=(int)len+1;
 for(int k=0;k<=n;k++)md_soft_point(x0+dx*k/n,y0+dy*k/n,ink);
}
static void ov_ellipse(const OrbitView *o,uint16_t ink,float step){ // kept between the title and the strip
 const float b=o->a*fm_sqrtf(1-o->e*o->e),cx=ov_cx-o->a*o->e*fm_cosf(o->w),cy=ov_cy-o->a*o->e*fm_sinf(o->w);
 if(!ov_band(cy-o->a,cy+o->a))return;
 const float c=fm_cosf(o->w),s=fm_sinf(o->w);const int n=(int)(6.2831853f*fm_sqrtf((o->a*o->a+b*b)*.5f)/step)+12;
 for(int k=0;k<n;k++){const float t=k*6.2831853f/n,ex=o->a*fm_cosf(t),ey=b*fm_sinf(t),y=cy+ex*s+ey*c;if(y>=ov_top&&y<=ov_bottom)md_soft_point(cx+ex*c-ey*s,y,ink);}
}
static void ov_fmt_period(int steps,char *s,int brief){ // in beats below a bar, in bars above
 const int bar=steps>=16,whole=bar?steps/16:steps/4,half=bar?(steps%16)!=0:(steps%4)!=0;
 md_num(whole,s);while(*s)s++;if(half){*s++='.';*s++='5';}
 const char *u=brief?(bar?" BAR":" BT"):bar?(steps==16?" BAR":" BARS"):(steps==4?" BEAT":" BEATS");while(*u)*s++=*u++;*s=0;
}
static void ov_fmt_milli(int v,char *s){md_num(v/1000,s);while(*s)s++;*s++='.';*s++=(char)('0'+v/100%10);*s++=(char)('0'+v/10%10);*s=0;}
static void ov_append(char *s,const char *a){while(*s)s++;while(*a)*s++=*a++;*s=0;}
static int ov_gcd(int a,int b){while(b){int t=a%b;a=b;b=t;}return a;}
// Steps until the comet next reaches the star (its mean anomaly comes round to 0).
static int ov_comet_wait(const md_patch *p){const float M=md_status()->comet_M;return (int)((1-M/6.2831853f)*md_orbit_comet_steps(p->orbit_comet));}
static const char *ov_pitch_name(int src){return src==MD_PITCH_MOTIF?"MOTIF":src==MD_PITCH_ECHO?"ECHO":"ORBIT";}
static void ov_fmt_bar(int num,int den,char *s){ // a fraction of a bar, reduced: 3/16, 1/4, 1 BAR
 const int g=ov_gcd(num,den);num/=g;den/=g;md_num(num,s);if(den==1){ov_append(s," BAR");return;}ov_append(s,"/");char b[8];md_num(den,b);ov_append(s,b);
}
// Label, value and what it does, for each knob of the two pages.
static void ov_knob(const md_patch *p,int page,int k,const char **label,char *value,char *info,int brief){
 const int tr=dream_ui.track;const md_orbit *o=&p->orbit[tr];info[0]=0;
 if(page==ORBIT){
  if(k==0){*label="PERIOD";
   if(md_orbit_tenths(o->period)){const int t=md_orbit_tenths(o->period);char d[3]={'.',(char)('0'+t%10),0};md_num(t/10,value);ov_append(value,d);ov_append(value," S");
    ov_append(info,"FREE: SECONDS, NEVER IN STEP");return;} // a free period: in seconds, whatever the tempo
   ov_fmt_period(md_orbit_steps(o->period),value,brief);
   int g=0,n=0,v[6];for(int i=0;i<6;i++)if((p->orbit[i].gates||i==tr)&&!md_orbit_tenths(p->orbit[i].period)){v[n]=md_orbit_steps(p->orbit[i].period);g=ov_gcd(g,v[n]);n++;}
   ov_append(info,"RATIO ");for(int i=0;i<n;i++){char b[8];md_num(v[i]/(g?g:1),b);if(i)ov_append(info,":");ov_append(info,b);}}
  else if(k==1){*label="ECCENTRICITY";if(!o->ecc){ov_append(value,"ROUND");ov_append(info,"EVEN BEATS: EUCLIDEAN");}
   else{ov_fmt_milli(o->ecc,value);const float e=o->ecc*.001f,f=(1+e)*(1+e)/((1-e)*(1-e));char b[12];md_num((int)f,b);ov_append(info,"X");ov_append(info,b);
    ov_append(info,".");b[0]=(char)('0'+(int)(f*10)%10);b[1]=0;ov_append(info,b);ov_append(info," FASTER AT THE STAR");}}
  else if(k==2){*label="GATES";if(!o->gates){ov_append(value,"OFF");ov_append(info,"BACK ON ITS RHYTHM PAGE");}
   else{float phi[MD_ORBIT_GATES_MAX];const int n=md_orbit_steps(o->period),g=md_orbit_gate_angles(p,tr,phi);md_num(g,value);char b[8];ov_append(info,"E(");md_num(g,b);ov_append(info,b);ov_append(info,",");md_num(n,b);ov_append(info,b);ov_append(info,") A TURN");}}
  else{*label="PHASE";md_num(o->phase*15,value);ov_append(info,"DEGREES: THE GATES TURN");}
 }else if(page==ORBIT_SYS){
  if(k==0){*label="PRECESSION";if(!p->orbit_prec){ov_append(value,"OFF");ov_append(info,"THE ELLIPSES STAY STILL");}
   else{md_num(p->orbit_prec,value);char b[8];ov_append(info,"1 TURN IN ");md_num((1600+p->orbit_prec/2)/p->orbit_prec,b);ov_append(info,b);ov_append(info," BARS");}}
  else if(k==1){*label="PERTURBATION";md_num(p->orbit_pert,value);ov_append(info,p->orbit_pert?"BREATHES, SKIPS, NEIGHBOURS":"PERFECT CLOCKWORK");}
  else if(k==2){*label="PITCH";ov_append(value,ov_pitch_name(o->pitch));
   if(o->pitch==MD_PITCH_ECHO){char b[4]={'T',(char)('1'+(tr+5)%6),0};ov_append(info,"TRIAD NOTE ABOVE ");ov_append(info,b);}
   else ov_append(info,o->pitch==MD_PITCH_MOTIF?"THE PHRASE, IN ORDER":"NEAR THE STAR = HIGHER");}
  else{*label="SYSTEM";ov_append(value,md_orbit_system_name(p->orbit_system,brief));ov_append(info,md_orbit_system_about(p->orbit_system));}
 }else if(page==ORBIT_MOONS){
  const int n=o->moons,spin=md_orbit_moon_steps(o->moon_spin);
  if(k==0){*label="MOONS";if(!n){ov_append(value,"OFF");ov_append(info,"NO ORNAMENT");}
   else{md_num(n,value);ov_append(info,n>1?"ORNAMENTS AFTER EACH NOTE":"ORNAMENT AFTER EACH NOTE");}}
  else if(k==1){*label="SPIN";ov_fmt_bar(spin,16,value);if(n){ov_append(info,"ONE EVERY ");char b[16];b[0]=0;ov_fmt_bar(spin,16*(n+1),b);ov_append(info,b);}else ov_append(info,"ONE TURN AFTER EACH NOTE");}
  else if(k==2){static const char *const brief_names[MD_MOON_INTERVALS]={"SAME","UP","DOWN","3RD+","3RD-","HARM"},
    *const names[MD_MOON_INTERVALS]={"REPEAT","STEPS UP","STEPS DOWN","THIRDS UP","THIRDS DOWN","HARMONICS"},
    *const infos[MD_MOON_INTERVALS]={"THE SAME NOTE AGAIN","UP THE SCALE, STEP BY STEP","DOWN THE SCALE, STEP BY STEP","UP THE SCALE IN THIRDS","DOWN THE SCALE IN THIRDS","OCTAVE, +FIFTH, 2 OCTAVES"};
   *label="INTERVAL";ov_append(value,(brief?brief_names:names)[o->moon_interval]);ov_append(info,infos[o->moon_interval]);}
  else{*label="LEVEL";md_num(o->moon_level,value);ov_append(info,"OF THE NOTE'S VELOCITY");}
 }else{
  if(k==0){*label="COMET";if(!p->orbit_comet){ov_append(value,"OFF");ov_append(info,"NO RARE EVENT");}
   else{md_num(md_orbit_comet_steps(p->orbit_comet)/16,value);ov_append(value,brief?" BAR":" BARS");
    if(!md_playing())ov_append(info,"FIRST PASS HALF A TURN IN");
    else if(md_status()->comet_now>0)ov_append(info,"PASSING THE STAR NOW");
    else{const int bars=(ov_comet_wait(p)+15)/16;char b[8];md_num(bars,b);ov_append(info,"NEXT PASS IN ");ov_append(info,b);ov_append(info,bars>1?" BARS":" BAR");}}}
  else if(k==1){static const char *const names[MD_COMET_EVENTS]={"SWELL","FREEZE","BOTH","FILTER"},*const infos[MD_COMET_EVENTS]={"THE ROOM SWELLS AS IT PASSES","THE GRANULAR TAPE FREEZES","THE ROOM SWELLS, THE TAPE FREEZES","A FILTER OPENS AS IT PASSES"};
   *label="EVENT";ov_append(value,names[p->comet_event]);ov_append(info,infos[p->comet_event]);}
  else if(k==2){*label="AMOUNT";md_num(p->comet_amount,value);ov_append(info,!p->comet_amount?"IT PASSES, NOTHING HAPPENS":p->comet_event==MD_COMET_FILTER?"HOW FAR THE FILTER CLOSES":"HOW MUCH ITS PASSAGE DOES");}
  else{static const char *const names[MD_CONJ_MODES]={"OFF","ACCENT","CHORD"},*const brief_names[MD_CONJ_MODES]={"OFF","ACC","CHORD"},
    *const infos[MD_CONJ_MODES]={"ALIGNED BODIES: NOTHING","ALIGNED: NEXT BEAT ACCENTED","ALIGNED: THEY STRIKE A CHORD"};
   *label="CONJUNCTION";ov_append(value,(brief?brief_names:names)[p->orbit_conj]);ov_append(info,infos[p->orbit_conj]);}
 }
}
// The MOONS page: the selected body close up, its outer side (away from the star) at the top, where each moon plays as
// the note's spin carries it past; each moon is labelled with the note it plays, the body with its own.
enum { MC_X=120, MC_Y=114, MC_R=42 };
static void ov_note_name(float pitch,char *s){const int n=(int)fm_floorf(pitch+.5f);s[0]=0;if(n<=0)return;ov_append(s,md_pitch_names[n%12]);char b[4];md_num(n/12-1,b);ov_append(s,b);}
static void ov_label(float x,float y,const char *s,uint16_t ink){if(*s&&ov_band(y-6,y+6))cv_text((int)(x-text_w(&FONT_XS,s)*.5f+.5f),(int)(y-4),&FONT_XS,s,ink);}
static void ov_closeup(const md_patch *p,int tr,int hk,uint32_t now){
 const md_orbit *o=&p->orbit[tr];const md_stats *ms=md_status();const uint16_t c=dream_colors[tr];const int n=o->moons,spun=ms->moon_t0[tr]>-100;
 float f=0;if(md_playing()&&spun){f=(ms->orbit_pos-ms->moon_t0[tr])/md_orbit_moon_steps(o->moon_spin);f=f<0?0:f>1?1:f;}
 // The star, below: a dotted line to it, clear of the labels.
 for(int y=MC_Y+24;y<182;y+=4)if((y<MC_Y+MC_R-6||y>MC_Y+MC_R+18)&&ov_band((float)y,(float)y))md_soft_point(MC_X,(float)y,md_tint(MD_BG,MD_INK,9));
 ov_ring(MC_X,186,4,md_tint(MD_BG,MD_INK,10),1.2f);md_circle(MC_X,186,1,MD_INK,1);
 // The moons' orbit, the outer side marked at the top (white as a moon plays there).
 const int lit=n&&ov_moon_k[tr]>0&&now-ov_moon_ms[tr]<260;
 ov_ring(MC_X,MC_Y,MC_R,md_tint(MD_BG,c,n?(hk==1?24u:14u):6u),n?1.3f:4);
 ov_seg(MC_X,MC_Y-MC_R-6,MC_X,MC_Y-MC_R+6,lit?MD_INK:md_tint(MD_BG,c,26));ov_seg(MC_X+1,MC_Y-MC_R-6,MC_X+1,MC_Y-MC_R+6,lit?MD_INK:md_tint(MD_BG,c,14));
 // The body: a ripple for each note, its note under it.
 if(dream_prefs.motion&&!ov_hit_skip[tr]&&now-ov_hit_ms[tr]<700&&md_playing()){const float k=(now-ov_hit_ms[tr])/700.0f;ov_ring(MC_X,MC_Y,9+20*fm_sqrtf(k),md_tint(MD_BG,c,(unsigned)(30*(1-k))),1.3f);}
 if(ov_band(MC_Y-8,MC_Y+8))md_circle(MC_X,MC_Y,7,o->gates?c:md_tint(MD_BG,c,12),1);
 {char s[8];ov_note_name(spun?ms->last_pitch[tr]:0,s);ov_label(MC_X,MC_Y+16,s,MD_DIM);}
 // The moons, parked behind the top until a note spins them once round; moon k passes the top k/(n+1) of the way.
 for(int k=0;k<n;k++){const float a=-1.5707963f+6.2831853f*(f-(k+1)/(float)(n+1)),x=MC_X+MC_R*fm_cosf(a),y=MC_Y+MC_R*fm_sinf(a);
  const int on=lit&&ov_moon_k[tr]==k+1;
  if(on&&dream_prefs.motion){const float q=(now-ov_moon_ms[tr])/260.0f;ov_ring(x,y,5+7*q,md_tint(MD_BG,MD_INK,(unsigned)(28*(1-q))),1.3f);}
  if(ov_band(y-4,y+4))md_circle((int)(x+.5f),(int)(y+.5f),on?4:3,on?MD_INK:md_tint(MD_BG,MD_INK,hk==0?30u:24u),1);
  char s[8];ov_note_name(spun?ms->moon_note[tr][k]:0,s);ov_label(MC_X+(MC_R+14)*fm_cosf(a),MC_Y+(MC_R+12)*fm_sinf(a),s,on?MD_INK:hk==2?c:MD_DIM);}
}
// The gates in the order the body will cross them (from where it is; stopped, from the top, gate there included), each
// with the phrase's next note: the engine's walk, from its cursor, rests skipped.
static int ov_motif_ok(const md_patch *p,int tr){const md_orbit *o=&p->orbit[tr];return o->gates&&o->pitch==MD_PITCH_MOTIF;}
static int ov_motif_empty(const md_patch *p,int tr){for(int k=0;k<16;k++)if(p->ext[tr].notes[k]>=0)return 0;return 1;}
static void ov_motif_labels(const md_patch *p,int tr,uint16_t col){
 const OrbitView *o=&ov[tr];if(!o->on||!ov_motif_ok(p,tr)||ov_motif_empty(p,tr))return;
 float phi[MD_ORBIT_GATES_MAX],delta[MD_ORBIT_GATES_MAX];int order[MD_ORBIT_GATES_MAX];const int n=md_orbit_gate_angles(p,tr,phi),playing=md_playing();
 for(int g=0;g<n;g++){float d=phi[g]-o->out+(playing?0:.02f);d-=6.2831853f*fm_floorf(d/6.2831853f);delta[g]=d-(playing?0:.02f);order[g]=g;}
 for(int a=1;a<n;a++)for(int b=a;b>0&&delta[order[b]]<delta[order[b-1]];b--){int t=order[b];order[b]=order[b-1];order[b-1]=t;}
 int at=playing?md_status()->motif_at[tr]:0;const int shown=n<8?n:8;
 for(int k=0;k<shown;k++){int found=-1;for(int j=0;j<16&&found<0;j++)if(p->ext[tr].notes[(at+j)%16]>=0)found=(at+j)%16;at=(found+1)%16;
  const float f=phi[order[k]],r=o->a*(1-o->e*o->e)/(1+o->e*fm_cosf(f-o->w))+11,x=ov_cx+r*fm_cosf(f),y=ov_cy+r*fm_sinf(f);if(y<48||y>192)continue;
  char s[8];ov_note_name(md_quantize_pitch((float)(p->track[tr].note+p->ext[tr].notes[found]+p->transpose),p->root,p->scale),s);
  ov_label(x,y,s,k==0?MD_INK:md_tint(MD_BG,col,(unsigned)(28-k*2)));}
}
static void md_orbit_page(const md_patch *p,uint16_t col){
 const int tr=dream_ui.track,page=dream_ui.page;const uint32_t now=plat_ms();
 const int touched=ov_touch>=0&&ov_touch_page==page&&now-ov_touch_ms<1500;const int hk=touched?ov_touch:-1;
 if(page==ORBIT_MOONS)ov_closeup(p,tr,hk,now);else{
 // The bar: a ring, four beats, a dot where the sequence is.
 ov_ring(ov_cx,ov_cy,OV_RING,md_tint(MD_BG,MD_EDGE,20),1.4f);
 for(int k=0;k<4;k++){const float a=-1.5707963f+k*1.5707963f;ov_seg(ov_cx+(OV_RING-3)*fm_cosf(a),ov_cy+(OV_RING-3)*fm_sinf(a),ov_cx+(OV_RING+3)*fm_cosf(a),ov_cy+(OV_RING+3)*fm_sinf(a),k?MD_EDGE:MD_DIM);}
 if(md_playing()){float b=ov_pos-16*fm_floorf(ov_pos/16);const float a=-1.5707963f+b*(6.2831853f/16);md_circle((int)(ov_cx+OV_RING*fm_cosf(a)+.5f),(int)(ov_cy+OV_RING*fm_sinf(a)+.5f),2,MD_INK,1);}
 // Ellipses: the selected one bright; with no gates, dotted and faint.
 for(int pass=0;pass<2;pass++)for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on||(i==tr)!=(pass==1))continue;
  const int lit=i==tr&&page==ORBIT&&(hk==0||hk==1);
  ov_ellipse(o,md_tint(MD_BG,dream_colors[i],p->orbit[i].gates?(i==tr?(lit?32u:22u):15u):7u),p->orbit[i].gates?(lit?.8f:1.2f):3.0f);
  // Where it is fastest (the periapsis), while the precession is being turned.
  if(page==ORBIT_SYS&&hk==0&&o->e>.01f){const float r=o->a*(1-o->e);md_circle((int)(ov_cx+r*fm_cosf(o->w)+.5f),(int)(ov_cy+r*fm_sinf(o->w)+.5f),2,dream_colors[i],0);}}
 // Gates: a short radial stroke on the ellipse, white while it plays, grey when refused.
 for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on||!p->orbit[i].gates)continue;float phi[MD_ORBIT_GATES_MAX];const int n=md_orbit_gate_angles(p,i,phi);
  const int lit_ms=ov_hit_gate[i]>=0&&now-ov_hit_ms[i]<260;
  for(int g=0;g<n;g++){const float r=o->a*(1-o->e*o->e)/(1+o->e*fm_cosf(phi[g]-o->w)),c=fm_cosf(phi[g]),s=fm_sinf(phi[g]);
   const int on=lit_ms&&g==ov_hit_gate[i],big=i==tr&&page==ORBIT&&(hk==2||hk==3);const float d=big?5:3.5f;
   const uint16_t ink=on?(ov_hit_skip[i]?MD_EDGE:MD_INK):md_tint(MD_BG,dream_colors[i],i==tr?30u:16u);
   ov_seg(ov_cx+(r-d)*c,ov_cy+(r-d)*s,ov_cx+(r+d)*c,ov_cy+(r+d)*s,ink);if(on||big)ov_seg(ov_cx+(r-d)*c+s*.7f,ov_cy+(r-d)*s-c*.7f,ov_cx+(r+d)*c+s*.7f,ov_cy+(r+d)*s-c*.7f,ink);}}
 // MOTIF: the notes the selected body will play, written by the gates it will cross next, in order (the next brightest).
 if(page!=ORBIT_COMET)ov_motif_labels(p,tr,col);
 // A conjunction: the line from the star through both bodies, fading over 1.2 s.
 if(ov_conj_a>=0&&now-ov_conj_ms<1200){const OrbitView *a=&ov[ov_conj_a],*b=&ov[ov_conj_b];const OrbitView *far=(a->x-ov_cx)*(a->x-ov_cx)+(a->y-ov_cy)*(a->y-ov_cy)>(b->x-ov_cx)*(b->x-ov_cx)+(b->y-ov_cy)*(b->y-ov_cy)?a:b;
  ov_seg(ov_cx,ov_cy,far->x,far->y,md_tint(MD_BG,MD_INK,(unsigned)(32-(now-ov_conj_ms)*26/1200)));}
 // ECHO: a dotted thread to the track it answers.
 for(int i=0;i<6;i++){const int s=(i+5)%6;if(!ov[i].on||!ov[s].on||p->orbit[i].pitch!=MD_PITCH_ECHO||!p->orbit[i].gates||!p->orbit[s].gates)continue;
  const float dx=ov[s].x-ov[i].x,dy=ov[s].y-ov[i].y;const int n=(int)(fm_sqrtf(dx*dx+dy*dy)/4);const uint16_t ink=md_tint(MD_BG,dream_colors[i],i==tr&&page==ORBIT_SYS&&hk==2?30u:14u);
  for(int k=1;k<n;k++)if(ov_band(ov[i].y+dy*k/n,ov[i].y+dy*k/n))md_soft_point(ov[i].x+dx*k/n,ov[i].y+dy*k/n,ink);}
 // Trails (their length is the speed), ripples, bodies.
 for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on)continue;const uint16_t c=dream_colors[i];const int has=p->orbit[i].gates>0;
  if(dream_prefs.motion&&has){float x=o->x,y=o->y;for(int k=0;k<OV_TRAIL;k++){ov_seg(x,y,o->tx[k],o->ty[k],md_tint(MD_BG,c,(unsigned)(32-k*28/OV_TRAIL)));ov_seg(x+.5f,y+.5f,o->tx[k]+.5f,o->ty[k]+.5f,md_tint(MD_BG,c,(unsigned)(20-k*18/OV_TRAIL)));x=o->tx[k];y=o->ty[k];}}
  if(dream_prefs.motion&&has&&!ov_hit_skip[i]&&now-ov_hit_ms[i]<700){const float k=(now-ov_hit_ms[i])/700.0f;ov_ring(ov_hit_x[i],ov_hit_y[i],4+(7+12*fm_minf(1,ov_hit_vel[i]*3))*fm_sqrtf(k),md_tint(MD_BG,c,(unsigned)(30*(1-k))),1.3f);}
  // Moons, where the last note's spin has taken them; lit as each plays (the MOONS page shows them close up).
  if(has&&p->orbit[i].moons&&ov_band(o->y-OV_MOON-3,o->y+OV_MOON+3))
   for(int k=0;k<p->orbit[i].moons;k++){const int lit=ov_moon_k[i]==k+1&&now-ov_moon_ms[i]<220;
    md_circle((int)(o->mx[k]+.5f),(int)(o->my[k]+.5f),lit?2:1,lit?MD_INK:md_tint(MD_BG,MD_INK,i==tr?28u:20u),1);}
  if(ov_band(o->y-8,o->y+8)){if(i==tr)md_circle((int)(o->x+.5f),(int)(o->y+.5f),6,md_tint(MD_BG,c,has?28u:12u),0);md_circle((int)(o->x+.5f),(int)(o->y+.5f),i==tr?4:3,has?c:md_tint(MD_BG,c,12),1);}}
 // The comet: its whole orbit while it is being set; inside the bar ring, itself and its tail (away from the star, as
 // long as its passage acts); beyond, a mark on the ring in its direction, brighter as it nears.
 if(ov_comet.on){const OrbitView *c=&ov_comet;
  if(page==ORBIT_COMET&&hk==0)ov_ellipse(c,md_tint(MD_BG,MD_INK,10),3);
  const float dx=c->x-ov_cx,dy=c->y-ov_cy,r=fm_sqrtf(dx*dx+dy*dy),co=fm_cosf(c->out),si=fm_sinf(c->out);
  if(r<OV_RING-4){const float tail=4+30*md_status()->comet_now;
   for(int k=0;k<3;k++){const float o=(k-1)*.8f;ov_seg(c->x-si*o,c->y+co*o,c->x+co*tail-si*o,c->y+si*tail+co*o,md_tint(MD_BG,MD_INK,k==1?18u:9u));}
   if(ov_band(c->y-3,c->y+3))md_circle((int)(c->x+.5f),(int)(c->y+.5f),2,MD_INK,1);}
  else{const float x=ov_cx+OV_RING*co,y=ov_cy+OV_RING*si,near=fm_minf(1,(OV_RING+80)/(r+1)-.5f);
   if(ov_band(y-3,y+3))md_circle((int)(x+.5f),(int)(y+.5f),2,md_tint(MD_BG,MD_INK,(unsigned)(10+16*fm_maxf(0,near))),0);}}
 // The star glows with what is playing; the perturbation shows as a halo while it is turned.
 {const float g=4+fm_minf(9,ov_level*30);ov_ring(ov_cx,ov_cy,g,md_tint(MD_BG,MD_INK,8),1.2f);ov_ring(ov_cx,ov_cy,g*.6f,md_tint(MD_BG,MD_INK,14),1.2f);
  if(page==ORBIT_SYS&&hk==1&&p->orbit_pert)ov_ring(ov_cx,ov_cy,10+p->orbit_pert*.12f,md_tint(MD_BG,MD_INK,16),3);
  md_circle(ov_cx,ov_cy,2,MD_INK,1);}
 }
 // Words only where they help.
 if(page==ORBIT_COMET){if(!p->orbit_comet)md_text(10,44,"K1 COMET: A RARE PASSAGE",MD_DIM);else if(!md_playing())md_text(10,44,"PLAY: IT STARTS FAR AWAY",MD_DIM);
  else if(md_status()->comet_filter_hz>0){ // FILTER: the cutoff now, and where it stands between 60 Hz and 18 kHz (log)
   const float hz=md_status()->comet_filter_hz;char t[24]="FILTER ",b[12];if(hz>=1000){md_num((int)(hz/1000),b);ov_append(t,b);ov_append(t,".");b[0]=(char)('0'+(int)(hz/100)%10);b[1]=0;ov_append(t,b);ov_append(t," KHZ");}
   else{md_num((int)hz,b);ov_append(t,b);ov_append(t," HZ");}md_text(10,44,t,MD_DIM);
   if(ov_band(48,53)){const int w=(int)(150*fm_logf(hz/60)/fm_logf(300));cv_rect(90,50,150,1,MD_EDGE);cv_rect(90,49,w<0?0:w>150?150:w,3,col);}}}
 else if(!p->orbit[tr].gates)md_text(10,44,page==ORBIT?"K3 GATES STARTS THIS ORBIT":"NO ORBIT: ORBIT PAGE, K3 GATES",MD_DIM);
 else if(page!=ORBIT_MOONS&&p->orbit[tr].pitch==MD_PITCH_MOTIF&&ov_motif_empty(p,tr))md_text(10,44,"MOTIF EMPTY: KEYS ON GENERATOR",MD_DIM);
 else if(page==ORBIT_MOONS&&!p->orbit[tr].moons)md_text(10,44,"K1 MOONS: ORNAMENTS AFTER NOTES",MD_DIM);
 else if(!md_playing())md_text(10,44,page==ORBIT_MOONS?"EACH NOTE SPINS ITS MOONS ONCE":"PLAY: ALL START FROM THE TOP",MD_DIM);
 // The strip: four knobs, value and a short gauge.
 {const int y=200;cv_rect(0,y-1,240,25,MD_BG);cv_rect(6,y-1,228,1,MD_EDGE);
  for(int k=0;k<4;k++){const char *label;char value[24]={0},info[48];ov_knob(p,page,k,&label,value,info,1);const int x=6+k*58;
   char key[4]={'K',(char)('1'+k),0};cv_text(x,y+2,&FONT_XS,key,k==hk?MD_INK:MD_EDGE);
   cv_text(x+text_w(&FONT_XS,key)+4,y+2,&FONT_XS,value,k==hk?col:MD_DIM);}}
 // The card of the knob being turned, in the corner opposite the body it changes.
 // A full-width card, on the half of the screen away from the body, with a thread to it.
 if(touched){const char *label;char value[24]={0},info[48];ov_knob(p,page,hk,&label,value,info,0);
  const OrbitView *o=&ov[tr];const int body=page==ORBIT||(page==ORBIT_SYS&&hk==2),y=page==ORBIT_MOONS||(body&&o->y<ov_cy)||(page==ORBIT_COMET&&ov_comet.on&&ov_comet.y<ov_cy)?164:42;
  char head[24]={'K',(char)('1'+hk),' ',0};ov_append(head,label);
  cv_rect(6,y,228,33,MD_CARD);cv_rect(6,y,228,1,col);cv_text(12,y+3,&FONT_XS,head,MD_DIM);
  cv_text(228-text_w(&FONT_B,value),y+2,&FONT_B,value,col);cv_text(12,y+18,&FONT_XS,info,MD_INK);
  if(body&&o->on&&page!=ORBIT_MOONS)ov_seg(o->x,o->y,o->x,y<ov_cy?(float)y+33:(float)y,md_tint(MD_BG,col,14));}
}
// The system in miniature, for the home page, the start-up and LOOK / STARTUP (orbit_frame sets where): the same engine
// state as the ORBIT page, without words. The ellipses, the gates (white as they play), each body where it is with a
// ripple for each note, a line for a conjunction, the comet while it is near, the star glowing with what plays; around
// it all, the SPACE macro as a ring of dust. arrive (0..1): at the start-up the bodies fall in from far away onto their
// own orbits, and the ellipses appear as they arrive.
static void md_orbit_mini(const md_patch *p,float arrive,int live){
 const int tr=dream_ui.track;const uint32_t now=plat_ms();const float fall=1+2.4f*(1-arrive)*(1-arrive);const unsigned fade=(unsigned)(32*arrive);
 if(live){const float r=ov_fit+3+p->macro[3]*.006f;const unsigned dust=(unsigned)(4+p->macro[3]*20/1000);
  if(ov_band(ov_cy-r,ov_cy+r)){const int n=(int)(r*1.6f);for(int k=0;k<n;k++){const float t=k*6.2831853f/n;md_soft_point(ov_cx+r*fm_cosf(t),ov_cy+r*fm_sinf(t),md_tint(MD_BG,MD_DIM,dust));}}}
 for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on)continue;const int has=p->orbit[i].gates>0,mute=p->track[i].mute;
  if(!has&&!live)continue;
  ov_ellipse(o,md_tint(MD_BG,mute?MD_EDGE:dream_colors[i],(has?(i==tr&&live?20u:12u):6u)*fade/32),has?1.5f:3.5f);
  if(!has)continue;
  float phi[MD_ORBIT_GATES_MAX];const int n=md_orbit_gate_angles(p,i,phi),lit=ov_hit_gate[i]>=0&&now-ov_hit_ms[i]<260;
  for(int g=0;g<n;g++){const float r=o->a*(1-o->e*o->e)/(1+o->e*fm_cosf(phi[g]-o->w)),x=ov_cx+r*fm_cosf(phi[g]),y=ov_cy+r*fm_sinf(phi[g]);
   if(!ov_band(y-2,y+2))continue;const int on=lit&&g==ov_hit_gate[i];
   if(on)md_circle((int)(x+.5f),(int)(y+.5f),1,ov_hit_skip[i]?MD_EDGE:MD_INK,1);else md_soft_point(x,y,md_tint(MD_BG,mute?MD_EDGE:dream_colors[i],22*fade/32));}}
 if(arrive>=1&&ov_conj_a>=0&&now-ov_conj_ms<1200){const OrbitView *a=&ov[ov_conj_a],*b=&ov[ov_conj_b];
  const OrbitView *far=(a->x-ov_cx)*(a->x-ov_cx)+(a->y-ov_cy)*(a->y-ov_cy)>(b->x-ov_cx)*(b->x-ov_cx)+(b->y-ov_cy)*(b->y-ov_cy)?a:b;
  ov_seg(ov_cx,ov_cy,far->x,far->y,md_tint(MD_BG,MD_INK,(unsigned)(26-(now-ov_conj_ms)*22/1200)));}
 for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on||!p->orbit[i].gates)continue;
  const uint16_t c=p->track[i].mute?MD_EDGE:dream_colors[i];const float x=ov_cx+(o->x-ov_cx)*fall,y=ov_cy+(o->y-ov_cy)*fall;
  if(dream_prefs.motion&&!ov_hit_skip[i]&&now-ov_hit_ms[i]<700&&md_playing()){const float k=(now-ov_hit_ms[i])/700.0f;ov_ring(x,y,3+(5+8*fm_minf(1,ov_hit_vel[i]*3))*fm_sqrtf(k),md_tint(MD_BG,c,(unsigned)(28*(1-k))),1.3f);}
  if(y<ov_top+3||y>ov_bottom-3||!ov_band(y-6,y+6))continue; // falling in: hidden until inside the frame
  if(live&&i==tr)md_circle((int)(x+.5f),(int)(y+.5f),5,md_tint(MD_BG,c,24),0);
  md_circle((int)(x+.5f),(int)(y+.5f),i==tr&&live?3:2,c,1);}
 if(ov_comet.on&&arrive>=1){const OrbitView *c=&ov_comet;const float dx=c->x-ov_cx,dy=c->y-ov_cy,r=fm_sqrtf(dx*dx+dy*dy);
  if(r<ov_fit){const float tail=3+18*md_status()->comet_now,co=fm_cosf(c->out),si=fm_sinf(c->out);
   ov_seg(c->x,c->y,c->x+co*tail,c->y+si*tail,md_tint(MD_BG,MD_INK,14));if(ov_band(c->y-2,c->y+2))md_circle((int)(c->x+.5f),(int)(c->y+.5f),1,MD_INK,1);}}
 {const float g=3+fm_minf(7,ov_level*24);ov_ring(ov_cx,ov_cy,g,md_tint(MD_BG,MD_INK,8),1.2f);ov_ring(ov_cx,ov_cy,g*.6f,md_tint(MD_BG,MD_INK,14),1.2f);
  if(ov_band(ov_cy-3,ov_cy+3))md_circle((int)ov_cx,(int)ov_cy,arrive<1?(int)(1+arrive):2,MD_INK,1);}
}
