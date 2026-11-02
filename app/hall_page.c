/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the HALL and HALL / COLOR pages. Included by app.c after orbit_page.c (its drawing helpers).
 * The decay as the engine computes it: three lines, how the low, middle and high parts of the tail fall 60 dB after
 * the pre-delay (flat when the decay is infinite). On each, a dot: the energy the tail has in that band now, placed
 * where the line reaches that level, so a dying tail slides down its line. */
enum { HG_X0=30, HG_X1=226, HG_Y0=62, HG_Y1=178 };
static md_hall hg;
static void hall_view_update(void){md_hall_view(&hg);}
static float hg_tmax(void){float t=1;for(int b=0;b<3;b++)if(hg.rt60[b]>t)t=hg.rt60[b];return hg.infinite?10:(t+hg.predelay_ms*.001f)*1.1f;}
static float hg_x(float t){return HG_X0+(HG_X1-HG_X0)*t/hg_tmax();}
static float hg_y(float db){return HG_Y0+(HG_Y1-HG_Y0)*fm_minf(1,fm_maxf(0,-db/60));}
static void hg_fmt_s(float s,char *out){out[0]=0;const int t=(int)(s*10+.5f);char b[8];md_num(t/10,b);ov_append(out,b);ov_append(out,".");b[0]=(char)('0'+t%10);b[1]=0;ov_append(out,b);ov_append(out," S");}
static void hall_knob(const md_patch *p,int page,int k,const char **label,char *value,char *info,int brief){
 info[0]=value[0]=0;char b[16];
 if(page==HALL){
  if(k==0){*label="DECAY";if(p->reverb_time>=1000){ov_append(value,brief?"INF":"INFINITE");ov_append(info,"THE HALL HOLDS, NOTHING ENTERS");}
   else{hg_fmt_s(md_hall_rt60(p->reverb_time),value);ov_append(info,"60 DB DOWN IN ");ov_append(info,value);}}
  else if(k==1){*label="SIZE";md_num(p->hall_size/10,value);const float ms=(.35f+.65f*p->hall_size*.001f)*10203.5f/(p->hall_vintage?22.05f:44.1f);
   ov_append(info,"ONE TURN OF THE LOOP: ");md_num((int)ms,b);ov_append(info,b);ov_append(info," MS");}
  else if(k==2){*label="PRE-DELAY";md_num(p->hall_predelay,value);ov_append(value," MS");ov_append(info,p->hall_predelay?"BEFORE THE HALL ANSWERS":"THE HALL ANSWERS AT ONCE");}
  else{*label="MOD";md_num(p->hall_mod/10,value);ov_append(info,p->hall_mod?"A SLOW SHIMMER IN THE TAIL":"A STILL TAIL");}
 }else{
  if(k==0){*label="BASS";const float x=hg.rt60[1]>0?hg.rt60[0]/hg.rt60[1]:1;const int c=(int)(x*10+.5f);md_num(c/10,value);ov_append(value,".");b[0]=(char)('0'+c%10);b[1]=0;ov_append(value,b);ov_append(value,"X");
   ov_append(info,"LOWS LAST ");ov_append(info,value);ov_append(info," AS LONG");}
  else if(k==1){*label="TREBLE";md_num(p->room_tone/10,value);if(hg.infinite)ov_append(info,"INFINITE: NO DAMPING");else{ov_append(info,"HIGHS DOWN 60 DB IN ");hg_fmt_s(hg.rt60[2],b);ov_append(info,b);}}
  else if(k==2){*label="DIFFUSION";md_num(p->room_diffusion/10,value);ov_append(info,p->room_diffusion<300?"CLEARER ECHOES IN THE ONSET":"A SMOOTH, DENSE ONSET");}
  else{*label="VINTAGE";ov_append(value,p->hall_vintage?(brief?"22K":"22 KHZ"):"OFF");ov_append(info,p->hall_vintage?"DARKER, TWICE THE HALL, HALF THE COST":"44.1 KHZ, THE FULL BAND");}
 }
}
static void md_hall_page(const md_patch *p,uint16_t col){
 const int page=dream_ui.page;const uint32_t now=plat_ms();
 const int touched=ov_touch>=0&&ov_touch_page==page&&now-ov_touch_ms<1500,hk=touched?ov_touch:-1;
 // Axes: 0 and -60 dB, a tick each second (each five beyond 12 s), the time at the end.
 ov_seg(HG_X0,HG_Y1,HG_X1,HG_Y1,MD_EDGE);ov_seg(HG_X0,HG_Y0,HG_X0,HG_Y1,MD_EDGE);
 {const float tm=hg_tmax();const int step=tm>12?5:1;for(int s=step;s<tm;s+=step){const float x=hg_x((float)s);ov_seg(x,HG_Y1,x,HG_Y1+3,MD_EDGE);}
  char t[12];hg_fmt_s(tm,t);cv_text(HG_X1-text_w(&FONT_XS,t),HG_Y1+5,&FONT_XS,t,MD_DIM);cv_text(HG_X0,HG_Y1+5,&FONT_XS,"0",MD_DIM);
  cv_text(4,HG_Y0-4,&FONT_XS,"0",MD_EDGE);cv_text(4,HG_Y1-8,&FONT_XS,"-60",MD_EDGE);}
 // Pre-delay: the start of every line, along the top.
 const float x0=hg_x(hg.predelay_ms*.001f);
 if(hg.predelay_ms>0)for(float x=HG_X0;x<x0;x+=3)md_soft_point(x,HG_Y0,page==HALL&&hk==2?col:MD_EDGE);
 // The three lines (low, middle, high), each with its band's energy now.
 static const char *const names[3]={"LOW","MID","HIGH"};
 for(int b=0;b<3;b++){
  const int lit=(page==HALL&&(hk==0||hk==1))||(page==HALL_COLOR&&((hk==0&&b==0)||(hk==1&&b==2)));
  const uint16_t ink=b==1?col:b==0?md_tint(MD_BG,col,lit?30u:20u):md_tint(MD_BG,MD_INK,lit?28u:16u);
  const float rt=hg.rt60[b],x1=rt<0?HG_X1:hg_x(hg.predelay_ms*.001f+rt),wob=page==HALL&&hk==3?p->hall_mod*.004f:0;
  const float y1=rt<0?HG_Y0:HG_Y1,dy=(y1-HG_Y0),dx=x1-x0;const int n=(int)(fm_sqrtf(dx*dx+dy*dy))+1;
  for(int k=0;k<=n;k++){const float f=(float)k/n,w=wob*fm_sinf(f*40);md_soft_point(x0+dx*f,HG_Y0+dy*f+w,ink);if(lit||b==1)md_soft_point(x0+dx*f,HG_Y0+dy*f+w+.7f,ink);}
  const float lx=rt<0?HG_X1-20:fm_minf(x1+2,HG_X1-18),ly=rt<0?HG_Y0+4+b*9:HG_Y1-12-b*9;cv_text((int)lx,(int)ly,&FONT_XS,names[b],ink);
  // The dot: the band's level now (0 here is -30 dB of energy), where the line has fallen that far.
  const float db=fm_log2f(hg.energy[b]+1e-12f)*3.0103f+30;
  if(db>-60){const float down=fm_minf(0,db),x=rt<0?x0+(HG_X1-x0)*.5f:x0+dx*(-down/60),y=rt<0?hg_y(down):HG_Y0+dy*(-down/60);
   if(ov_band(y-3,y+3))md_circle((int)(x+.5f),(int)(y+.5f),2,ink,1);}}
 // Words only where they help.
 if(!p->fx_level)md_text(10,44,"RETURN AT 0: FX > SPACE, K4",MD_DIM);
 else{int sends=0;for(int i=0;i<6;i++)sends+=p->ext[i].reverb_send;if(!sends)md_text(10,44,"NO SEND: EDIT > MIXER, K4",MD_DIM);
  else if(hg.infinite)md_text(10,44,"INFINITE: THE TAIL HOLDS",MD_DIM);else if(hg.vintage)md_text(10,44,"VINTAGE: 22 KHZ",MD_DIM);}
 // The strip, and the card of the knob being turned (at the bottom for PRE-DELAY, which sits at the top left).
 {const int y=200;cv_rect(0,y-1,240,25,MD_BG);cv_rect(6,y-1,228,1,MD_EDGE);
  for(int k=0;k<4;k++){const char *label;char value[24],info[48];hall_knob(p,page,k,&label,value,info,1);const int x=6+k*58;
   char key[4]={'K',(char)('1'+k),0};cv_text(x,y+2,&FONT_XS,key,k==hk?MD_INK:MD_EDGE);cv_text(x+text_w(&FONT_XS,key)+4,y+2,&FONT_XS,value,k==hk?col:MD_DIM);}}
 if(touched){const char *label;char value[24],info[48];hall_knob(p,page,hk,&label,value,info,0);const int y=page==HALL&&hk==2?164:42;
  char head[24]={'K',(char)('1'+hk),' ',0};ov_append(head,label);
  cv_rect(6,y,228,33,MD_CARD);cv_rect(6,y,228,1,col);cv_text(12,y+3,&FONT_XS,head,MD_DIM);
  cv_text(228-text_w(&FONT_B,value),y+2,&FONT_B,value,col);cv_text(12,y+18,&FONT_XS,info,MD_INK);}
}
