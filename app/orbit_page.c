/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the ORBIT and ORBIT / SYSTEM pages. Included by app.c before md_draw_all.
 * One large drawing, a thin strip for K1-K4, and the card of the knob being turned in the corner opposite what it
 * changes. Everything drawn is the engine's state: the ellipses as they are now (precession, breathing), the gates
 * where the bodies play, each body where it is with a trail whose length is its speed (the rubato), a ripple for each
 * note, the gate lit when it plays (grey when the perturbation refused it), a line for a conjunction, the bar on the
 * outer ring. The geometry is computed once a frame (orbit_view_update), then drawn band by band. */
enum { OV_CX=120, OV_CY=119, OV_RING=78, OV_TRAIL=10 };
typedef struct { int on; float a,e,w,x,y,tx[OV_TRAIL],ty[OV_TRAIL]; } OrbitView;
static OrbitView ov[6];
static float ov_pos,ov_level;
static uint32_t ov_hits_seen[6],ov_hit_ms[6],ov_conj_seen,ov_conj_ms,ov_touch_ms;
static int ov_hit_gate[6]={-1,-1,-1,-1,-1,-1},ov_hit_skip[6],ov_conj_a=-1,ov_conj_b=-1,ov_touch=-1,ov_touch_page=-1;
static float ov_hit_x[6],ov_hit_y[6],ov_hit_vel[6];

static void orbit_touch(int knob){ov_touch=knob;ov_touch_page=dream_ui.page;ov_touch_ms=plat_ms();}
// Where a body is, from its mean anomaly (Kepler's equation), on the screen: x right, y down, the star at the focus.
static void ov_point(const OrbitView *o,float M,float *x,float *y){
 const float E=md_orbit_eccentric(M,o->e),ex=o->a*(fm_cosf(E)-o->e),ey=o->a*fm_sqrtf(1-o->e*o->e)*fm_sinf(E);
 const float c=fm_cosf(o->w),s=fm_sinf(o->w);*x=OV_CX+ex*c-ey*s;*y=OV_CY+ex*s+ey*c;
}
// The mean anomaly where a body sits at direction phi (stopped: all at the top, where PLAY starts them).
static float ov_mean_at(float phi,float w,float e){const float t=(phi-w)*.5f,E=2*md_atan2(fm_sqrtf(1-e)*fm_sinf(t),fm_sqrtf(1+e)*fm_cosf(t));return E-e*fm_sinf(E);}
static void orbit_view_update(void){
 const md_patch *p=md_state();const md_stats *ms=md_status();float p23[6],lo=1e9f,hi=0,ext=1;
 ov_pos=ms->orbit_pos;
 // Farther out = slower (Kepler's third law, a ~ T^(2/3)), squeezed between 24 and 62 pixels, then fitted to the ring.
 for(int i=0;i<6;i++){p23[i]=fm_powf((float)md_orbit_steps(p->orbit[i].period),.6666667f);if(p->orbit[i].gates){lo=fminf(lo,p23[i]);hi=fmaxf(hi,p23[i]);}}
 for(int i=0;i<6;i++){OrbitView *o=&ov[i];o->on=p->orbit[i].gates>0||i==dream_ui.track;
  o->a=hi-lo<1e-3f?46:24+38*(p23[i]-lo)/(hi-lo);if(o->a<24)o->a=24;if(o->a>62)o->a=62;
  o->e=md_playing()?ms->orbit_e[i]:p->orbit[i].ecc*.001f;o->w=ms->orbit_w[i];
  if(o->on&&o->a*(1+o->e)>ext)ext=o->a*(1+o->e);}
 const float fit=fminf(1.2f,72/ext);
 for(int i=0;i<6;i++){OrbitView *o=&ov[i];o->a*=fit;if(!o->on)continue;
  const float M=md_playing()?ms->orbit_M[i]:ov_mean_at(-1.5707963f,o->w,o->e);ov_point(o,M,&o->x,&o->y);
  for(int k=0;k<OV_TRAIL;k++)ov_point(o,M-(k+1)*(6.2831853f/9/OV_TRAIL),&o->tx[k],&o->ty[k]); // a ninth of a turn behind
  if(ms->orbit_hits[i]!=ov_hits_seen[i]||ms->orbit_hit_gate[i]!=ov_hit_gate[i]){ // a note (or a refused gate) since the last frame
   if(ms->orbit_hits[i]!=ov_hits_seen[i]){ov_hit_ms[i]=plat_ms();ov_hit_x[i]=o->x;ov_hit_y[i]=o->y;ov_hit_vel[i]=ms->track_peak[i];}
   ov_hits_seen[i]=ms->orbit_hits[i];ov_hit_gate[i]=ms->orbit_hit_gate[i];ov_hit_skip[i]=ms->orbit_hit_skip[i];if(ov_hit_skip[i])ov_hit_ms[i]=plat_ms();}}
 if(ms->conj_count!=ov_conj_seen){ov_conj_seen=ms->conj_count;ov_conj_ms=plat_ms();ov_conj_a=ms->conj_a;ov_conj_b=ms->conj_b;}
 ov_level=fmaxf(ms->out_peak[0],ms->out_peak[1]);
}
static int ov_band(float y0,float y1){const float top=-(float)cv_oy;return y1>=top-2&&y0<top+42;} // does [y0, y1] touch this band
static void ov_ring(float cx,float cy,float r,uint16_t ink,float step){
 if(r<=0||!ov_band(cy-r,cy+r))return;
 const int n=(int)(6.2831853f*r/step)+8;for(int k=0;k<n;k++){const float t=k*6.2831853f/n;md_soft_point(cx+r*fm_cosf(t),cy+r*fm_sinf(t),ink);}
}
static void ov_seg(float x0,float y0,float x1,float y1,uint16_t ink){ // an antialiased line, as dots a pixel apart
 if(!ov_band(fminf(y0,y1),fmaxf(y0,y1)))return;
 const float dx=x1-x0,dy=y1-y0,len=fm_sqrtf(dx*dx+dy*dy);const int n=(int)len+1;
 for(int k=0;k<=n;k++)md_soft_point(x0+dx*k/n,y0+dy*k/n,ink);
}
static void ov_ellipse(const OrbitView *o,uint16_t ink,float step){
 const float b=o->a*fm_sqrtf(1-o->e*o->e),cx=OV_CX-o->a*o->e*fm_cosf(o->w),cy=OV_CY-o->a*o->e*fm_sinf(o->w);
 if(!ov_band(cy-o->a,cy+o->a))return;
 const float c=fm_cosf(o->w),s=fm_sinf(o->w);const int n=(int)(6.2831853f*fm_sqrtf((o->a*o->a+b*b)*.5f)/step)+12;
 for(int k=0;k<n;k++){const float t=k*6.2831853f/n,ex=o->a*fm_cosf(t),ey=b*fm_sinf(t);md_soft_point(cx+ex*c-ey*s,cy+ex*s+ey*c,ink);}
}
static void ov_fmt_period(int steps,char *s,int brief){ // in beats below a bar, in bars above
 const int bar=steps>=16,whole=bar?steps/16:steps/4,half=bar?(steps%16)!=0:(steps%4)!=0;
 md_num(whole,s);while(*s)s++;if(half){*s++='.';*s++='5';}
 const char *u=brief?(bar?" BAR":" BT"):bar?(steps==16?" BAR":" BARS"):(steps==4?" BEAT":" BEATS");while(*u)*s++=*u++;*s=0;
}
static void ov_fmt_milli(int v,char *s){md_num(v/1000,s);while(*s)s++;*s++='.';*s++=(char)('0'+v/100%10);*s++=(char)('0'+v/10%10);*s=0;}
static void ov_append(char *s,const char *a){while(*s)s++;while(*a)*s++=*a++;*s=0;}
static int ov_gcd(int a,int b){while(b){int t=a%b;a=b;b=t;}return a;}
static const char *ov_pitch_name(int src){return src==MD_PITCH_MOTIF?"MOTIF":src==MD_PITCH_ECHO?"ECHO":"ORBIT";}
// Label, value and what it does, for each knob of the two pages.
static void ov_knob(const md_patch *p,int page,int k,const char **label,char *value,char *info,int brief){
 const int tr=dream_ui.track;const md_orbit *o=&p->orbit[tr];info[0]=0;
 if(page==ORBIT){
  if(k==0){*label="PERIOD";ov_fmt_period(md_orbit_steps(o->period),value,brief);
   int g=0,n=0,v[6];for(int i=0;i<6;i++)if(p->orbit[i].gates||i==tr){v[n]=md_orbit_steps(p->orbit[i].period);g=ov_gcd(g,v[n]);n++;}
   ov_append(info,"RATIO ");for(int i=0;i<n;i++){char b[8];md_num(v[i]/(g?g:1),b);if(i)ov_append(info,":");ov_append(info,b);}}
  else if(k==1){*label="ECCENTRICITY";if(!o->ecc){ov_append(value,"ROUND");ov_append(info,"EVEN BEATS: EUCLIDEAN");}
   else{ov_fmt_milli(o->ecc,value);const float e=o->ecc*.001f,f=(1+e)*(1+e)/((1-e)*(1-e));char b[12];md_num((int)f,b);ov_append(info,"X");ov_append(info,b);
    ov_append(info,".");b[0]=(char)('0'+(int)(f*10)%10);b[1]=0;ov_append(info,b);ov_append(info," FASTER AT THE STAR");}}
  else if(k==2){*label="GATES";if(!o->gates){ov_append(value,"OFF");ov_append(info,"BACK ON ITS RHYTHM PAGE");}
   else{const int n=md_orbit_steps(o->period),g=o->gates<n?o->gates:n;md_num(g,value);char b[8];ov_append(info,"E(");md_num(g,b);ov_append(info,b);ov_append(info,",");md_num(n,b);ov_append(info,b);ov_append(info,") A TURN");}}
  else{*label="PHASE";md_num(o->phase*15,value);ov_append(info,"DEGREES: THE GATES TURN");}
 }else{
  if(k==0){*label="PRECESSION";if(!p->orbit_prec){ov_append(value,"OFF");ov_append(info,"THE ELLIPSES STAY STILL");}
   else{md_num(p->orbit_prec,value);char b[8];ov_append(info,"1 TURN IN ");md_num((1600+p->orbit_prec/2)/p->orbit_prec,b);ov_append(info,b);ov_append(info," BARS");}}
  else if(k==1){*label="PERTURBATION";md_num(p->orbit_pert,value);ov_append(info,p->orbit_pert?"BREATHES, SKIPS, NEIGHBOURS":"PERFECT CLOCKWORK");}
  else if(k==2){*label="PITCH";ov_append(value,ov_pitch_name(o->pitch));
   if(o->pitch==MD_PITCH_ECHO){char b[4]={'T',(char)('1'+(tr+5)%6),0};ov_append(info,"TRIAD NOTE ABOVE ");ov_append(info,b);}
   else ov_append(info,o->pitch==MD_PITCH_MOTIF?"THE PHRASE, IN ORDER":"NEAR THE STAR = HIGHER");}
  else{*label="NOTE";const int n=p->track[tr].note;ov_append(value,md_pitch_names[n%12]);char b[4];md_num(n/12-1,b);ov_append(value,b);ov_append(info,"BASE NOTE OF THE TRACK");}
 }
}
static void md_orbit_page(const md_patch *p,uint16_t col){
 const int tr=dream_ui.track,page=dream_ui.page;const uint32_t now=plat_ms();
 const int touched=ov_touch>=0&&ov_touch_page==page&&now-ov_touch_ms<1500;const int hk=touched?ov_touch:-1;
 // The bar: a ring, four beats, a dot where the sequence is.
 ov_ring(OV_CX,OV_CY,OV_RING,md_tint(MD_BG,MD_EDGE,20),1.4f);
 for(int k=0;k<4;k++){const float a=-1.5707963f+k*1.5707963f;ov_seg(OV_CX+(OV_RING-3)*fm_cosf(a),OV_CY+(OV_RING-3)*fm_sinf(a),OV_CX+(OV_RING+3)*fm_cosf(a),OV_CY+(OV_RING+3)*fm_sinf(a),k?MD_EDGE:MD_DIM);}
 if(md_playing()){float b=ov_pos-16*fm_floorf(ov_pos/16);const float a=-1.5707963f+b*(6.2831853f/16);md_circle((int)(OV_CX+OV_RING*fm_cosf(a)+.5f),(int)(OV_CY+OV_RING*fm_sinf(a)+.5f),2,MD_INK,1);}
 // Ellipses: the selected one bright; with no gates, dotted and faint.
 for(int pass=0;pass<2;pass++)for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on||(i==tr)!=(pass==1))continue;
  const int lit=i==tr&&page==ORBIT&&(hk==0||hk==1);
  ov_ellipse(o,md_tint(MD_BG,dream_colors[i],p->orbit[i].gates?(i==tr?(lit?32u:22u):15u):7u),p->orbit[i].gates?(lit?.8f:1.2f):3.0f);
  // Where it is fastest (the periapsis), while the precession is being turned.
  if(page==ORBIT_SYS&&hk==0&&o->e>.01f){const float r=o->a*(1-o->e);md_circle((int)(OV_CX+r*fm_cosf(o->w)+.5f),(int)(OV_CY+r*fm_sinf(o->w)+.5f),2,dream_colors[i],0);}}
 // Gates: a short radial stroke on the ellipse, white while it plays, grey when refused.
 for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on||!p->orbit[i].gates)continue;float phi[MD_ORBIT_GATES_MAX];const int n=md_orbit_gate_angles(p,i,phi);
  const int lit_ms=ov_hit_gate[i]>=0&&now-ov_hit_ms[i]<260;
  for(int g=0;g<n;g++){const float r=o->a*(1-o->e*o->e)/(1+o->e*fm_cosf(phi[g]-o->w)),c=fm_cosf(phi[g]),s=fm_sinf(phi[g]);
   const int on=lit_ms&&g==ov_hit_gate[i],big=i==tr&&page==ORBIT&&(hk==2||hk==3);const float d=big?5:3.5f;
   const uint16_t ink=on?(ov_hit_skip[i]?MD_EDGE:MD_INK):md_tint(MD_BG,dream_colors[i],i==tr?30u:16u);
   ov_seg(OV_CX+(r-d)*c,OV_CY+(r-d)*s,OV_CX+(r+d)*c,OV_CY+(r+d)*s,ink);if(on||big)ov_seg(OV_CX+(r-d)*c+s*.7f,OV_CY+(r-d)*s-c*.7f,OV_CX+(r+d)*c+s*.7f,OV_CY+(r+d)*s-c*.7f,ink);}}
 // A conjunction: the line from the star through both bodies, fading over 1.2 s.
 if(ov_conj_a>=0&&now-ov_conj_ms<1200){const OrbitView *a=&ov[ov_conj_a],*b=&ov[ov_conj_b];const OrbitView *far=(a->x-OV_CX)*(a->x-OV_CX)+(a->y-OV_CY)*(a->y-OV_CY)>(b->x-OV_CX)*(b->x-OV_CX)+(b->y-OV_CY)*(b->y-OV_CY)?a:b;
  ov_seg(OV_CX,OV_CY,far->x,far->y,md_tint(MD_BG,MD_INK,(unsigned)(32-(now-ov_conj_ms)*26/1200)));}
 // ECHO: a dotted thread to the track it answers.
 for(int i=0;i<6;i++){const int s=(i+5)%6;if(!ov[i].on||!ov[s].on||p->orbit[i].pitch!=MD_PITCH_ECHO||!p->orbit[i].gates||!p->orbit[s].gates)continue;
  const float dx=ov[s].x-ov[i].x,dy=ov[s].y-ov[i].y;const int n=(int)(fm_sqrtf(dx*dx+dy*dy)/4);const uint16_t ink=md_tint(MD_BG,dream_colors[i],i==tr&&page==ORBIT_SYS&&hk==2?30u:14u);
  for(int k=1;k<n;k++)if(ov_band(ov[i].y+dy*k/n,ov[i].y+dy*k/n))md_soft_point(ov[i].x+dx*k/n,ov[i].y+dy*k/n,ink);}
 // Trails (their length is the speed), ripples, bodies.
 for(int i=0;i<6;i++){const OrbitView *o=&ov[i];if(!o->on)continue;const uint16_t c=dream_colors[i];const int has=p->orbit[i].gates>0;
  if(dream_prefs.motion&&has){float x=o->x,y=o->y;for(int k=0;k<OV_TRAIL;k++){ov_seg(x,y,o->tx[k],o->ty[k],md_tint(MD_BG,c,(unsigned)(32-k*28/OV_TRAIL)));ov_seg(x+.5f,y+.5f,o->tx[k]+.5f,o->ty[k]+.5f,md_tint(MD_BG,c,(unsigned)(20-k*18/OV_TRAIL)));x=o->tx[k];y=o->ty[k];}}
  if(dream_prefs.motion&&has&&!ov_hit_skip[i]&&now-ov_hit_ms[i]<700){const float k=(now-ov_hit_ms[i])/700.0f;ov_ring(ov_hit_x[i],ov_hit_y[i],4+(7+12*fminf(1,ov_hit_vel[i]*3))*fm_sqrtf(k),md_tint(MD_BG,c,(unsigned)(30*(1-k))),1.3f);}
  if(ov_band(o->y-8,o->y+8)){if(i==tr)md_circle((int)(o->x+.5f),(int)(o->y+.5f),6,md_tint(MD_BG,c,has?28u:12u),0);md_circle((int)(o->x+.5f),(int)(o->y+.5f),i==tr?4:3,has?c:md_tint(MD_BG,c,12),1);}}
 // The star glows with what is playing; the perturbation shows as a halo while it is turned.
 {const float g=4+fminf(9,ov_level*30);ov_ring(OV_CX,OV_CY,g,md_tint(MD_BG,MD_INK,8),1.2f);ov_ring(OV_CX,OV_CY,g*.6f,md_tint(MD_BG,MD_INK,14),1.2f);
  if(page==ORBIT_SYS&&hk==1&&p->orbit_pert)ov_ring(OV_CX,OV_CY,10+p->orbit_pert*.12f,md_tint(MD_BG,MD_INK,16),3);
  md_circle(OV_CX,OV_CY,2,MD_INK,1);}
 // Words only where they help.
 if(!p->orbit[tr].gates)md_text(10,44,"K3 GATES STARTS THIS ORBIT",MD_DIM);
 else if(!md_playing())md_text(10,44,"PLAY: ALL START FROM THE TOP",MD_DIM);
 // The strip: four knobs, value and a short gauge.
 {const int y=200;cv_rect(0,y-1,240,25,MD_BG);cv_rect(6,y-1,228,1,MD_EDGE);
  for(int k=0;k<4;k++){const char *label;char value[24]={0},info[48];ov_knob(p,page,k,&label,value,info,1);const int x=6+k*58;
   char key[4]={'K',(char)('1'+k),0};cv_text(x,y+2,&FONT_XS,key,k==hk?MD_INK:MD_EDGE);
   cv_text(x+text_w(&FONT_XS,key)+4,y+2,&FONT_XS,value,k==hk?col:MD_DIM);}}
 // The card of the knob being turned, in the corner opposite the body it changes.
 // A full-width card, on the half of the screen away from the body, with a thread to it.
 if(touched){const char *label;char value[24]={0},info[48];ov_knob(p,page,hk,&label,value,info,0);
  const OrbitView *o=&ov[tr];const int body=page==ORBIT||hk>=2,y=body&&o->y<OV_CY?164:42;
  char head[24]={'K',(char)('1'+hk),' ',0};ov_append(head,label);
  cv_rect(6,y,228,33,MD_CARD);cv_rect(6,y,228,1,col);cv_text(12,y+3,&FONT_XS,head,MD_DIM);
  cv_text(228-text_w(&FONT_B,value),y+2,&FONT_B,value,col);cv_text(12,y+18,&FONT_XS,info,MD_INK);
  if(body&&o->on)ov_seg(o->x,o->y,o->x,y<OV_CY?(float)y+33:(float)y,md_tint(MD_BG,col,14));}
}
