/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: Beads on screen. Included by app.c after orbit_page.c (its drawing helpers).
 * md_beads_ribbon: the memory as it is, now at the right edge and its whole length to the left: the level written
 * along it, the live grains (a band from where each reads, as long as it has left, white at its read point), the
 * bracket where new grains start (POSITION) and the feedback line under it. md_beads_page (BEADS): the ribbon, then
 * the grain envelope SHAPE really draws, the grain rate DENSITY asks for, and the media with the memory it gives. */
static const char *const bd_media_names[MD_BEADS_MEDIAS]={"DIRTY TAPE","CLEAN TAPE","DIGITAL"};
static const char *const bd_media_brief[MD_BEADS_MEDIAS]={"DIRTY","CLEAN","DIGI"};
static const char *const bd_media_infos[MD_BEADS_MEDIAS]={"MU-LAW, HALF RATE, WOW: ","12 BITS, HALF RATE: ","16 BITS: "};
static void md_beads_ribbon(const md_patch *p,uint16_t col,int top){
 static md_tape tv;md_tape_view(&tv);
 const int x0=8,w=224,half=19,mid=top+21;const float len=(float)(tv.length?tv.length:1),scale=(float)w/len,space=p->macro[3]*.001f;
 const uint16_t ice=RGB(151,200,230),head_ink=tv.freeze?ice:col;
 cv_rect(4,top-2,232,52,MD_BG);
 for(int i=0;i<MD_TAPE_GRAINS;i++){if(!tv.grain[i].active)continue;
  const int gx=x0+w-1-(int)(tv.grain[i].behind*scale);int gw=(int)(tv.grain[i].len*scale);if(gw<2)gw=2;
  unsigned mix=(unsigned)(3+tv.grain[i].env*space*10);if(mix>13)mix=13; // light: the memory shows through
  int a=gx,z=gx+gw;if(a<x0)a=x0;if(z>x0+w)z=x0+w;if(z>a)cv_rect(a,top,z-a,2*half+1,md_tint(MD_BG,col,mix));}
 // The level written along the memory (the write head moves toward lower indices: older is at head + behind).
 for(int px=0;px<w;px++){const float behind=(w-1-px)/scale;const unsigned pos=(unsigned)(tv.head+(unsigned)behind)%(unsigned)len;
  const unsigned q=tv.peak[(unsigned)(pos*256.0f/len)&255]*half*half/255;unsigned h=0;while((h+1)*(h+1)<=q)h++; // square-root scale
  cv_rect(x0+px,mid-(int)h,1,2*(int)h+1,tv.freeze?md_tint(MD_BG,ice,22):MD_DIM);}
 for(int i=0;i<MD_TAPE_GRAINS;i++){if(!tv.grain[i].active)continue;const int gx=x0+w-1-(int)(tv.grain[i].behind*scale);
  if(gx>=x0)cv_rect(gx,top,1,2*half+1,md_tint(MD_BG,MD_INK,(unsigned)(8+tv.grain[i].env*24)));}
 cv_rect(x0+w-2,top-2,2,2*half+5,head_ink);
 {int zw=(int)(tv.zone_len*scale);if(zw<2)zw=2;const int zx=x0+w-1-(int)(tv.zone*scale)-zw/2;
  cv_rect(zx<x0?x0:zx,top+2*half+3,zw,2,col);
  if(p->feedback){const int fx=zx+zw/2;const uint16_t fbk=md_tint(MD_BG,col,(unsigned)(4+p->feedback*24/1000));
   cv_rect(fx,top+2*half+7,x0+w-2-fx,1,fbk);cv_rect(x0+w-4,top+2*half+6,1,3,fbk);}}
 char s[12]="-";{const int c=(int)(md_beads_seconds()*10+.5f);char b[8];md_num(c/10,b);ov_append(s,b);ov_append(s,".");b[0]=(char)('0'+c%10);b[1]=0;ov_append(s,b);ov_append(s," S");}
 md_text(8,top+51,s,MD_DIM);md_text(84,top+51,tv.freeze?"FROZEN":p->reverse?"< REVERSE":"RECORDING",tv.freeze?ice:MD_INK);md_text(206,top+51,"NOW",MD_DIM);
}
static void bd_knob(const md_patch *p,int k,const char **label,char *value,char *info,int brief){
 info[0]=value[0]=0;char b[16];
 if(k==0){*label="DENSITY";const int d=p->beads_density;
  if(d>=480&&d<=520){ov_append(value,"NONE");ov_append(info,"NO NEW GRAIN");}
  else{const float hz=md_beads_rate(d);md_num(d<480?(480-d)*100/480:(d-520)*100/480,value);ov_append(info,d<480?"REGULAR, ":"RANDOM, ABOUT ");
   if(hz>=1){md_num((int)(hz+.5f),b);ov_append(info,b);ov_append(info," A SECOND");}else{md_num((int)(1/hz+.5f),b);ov_append(info,"ONE EVERY ");ov_append(info,b);ov_append(info," S");}}}
 else if(k==1){*label="SHAPE";md_num(p->beads_shape/10,value);const int s=p->beads_shape;
  ov_append(info,s<120?"SQUARE: ALL SUSTAIN":s<330?"SUSTAIN, THEN DECAY":s<500?"A SHORT ATTACK, A DECAY":s<760?"A BELL":"REVERSED: SWELLS, STOPS");}
 else if(k==2){*label="RANDOM";md_num(p->beads_random/10,value);ov_append(info,p->beads_random?"TIME, SIZE, SHAPE, PITCH WANDER":"EVERY GRAIN THE SAME");}
 else{*label="MEDIA";ov_append(value,(brief?bd_media_brief:bd_media_names)[p->beads_media]);ov_append(info,bd_media_infos[p->beads_media]);
  const int c=(int)(md_beads_seconds()*10+.5f);md_num(c/10,b);ov_append(info,b);ov_append(info,".");b[0]=(char)('0'+c%10);b[1]=0;ov_append(info,b);ov_append(info," S OF MEMORY");}
}
static void md_beads_page(const md_patch *p,uint16_t col){
 const uint32_t now=plat_ms();const int touched=ov_touch>=0&&ov_touch_page==BEADS&&now-ov_touch_ms<1500,hk=touched?ov_touch:-1;
 md_beads_ribbon(p,col,64);
 // The grain envelope, as SHAPE makes it (attack, sustain, release, smoothness), over a grain's length.
 {const int x0=40,x1=200,y0=128,y1=176;const uint16_t ink=hk==1?col:md_tint(MD_BG,col,22);
  ov_seg(x0,y1,x1,y1,MD_EDGE);float px=x0,py=y1;
  for(int k=0;k<=80;k++){const float t=k/80.0f,v=md_beads_envelope(p->beads_shape*.001f,t),x=x0+(x1-x0)*t,y=y1-(y1-y0)*v;
   ov_seg(px,py,x,y,ink);px=x;py=y;}
  ov_label(x0-16,y1-4,"0",MD_EDGE);ov_label(x1+14,y1-4,"END",MD_EDGE);}
 // Words only where they help.
 if(!p->macro[3])md_text(10,44,"SPACE AT 0: LIVE K4, GRAIN K1",MD_DIM);
 else{int sends=0;for(int i=0;i<6;i++)sends+=p->track[i].send;if(!sends)md_text(10,44,"NO SEND: EDIT > MIX, K2",MD_DIM);
  else if(p->beads_density>=480&&p->beads_density<=520)md_text(10,44,"DENSITY IN THE MIDDLE: NO GRAIN",MD_DIM);}
 {const int y=200;cv_rect(0,y-1,240,25,MD_BG);cv_rect(6,y-1,228,1,MD_EDGE);
  for(int k=0;k<4;k++){const char *label;char value[24],info[48];bd_knob(p,k,&label,value,info,1);const int x=6+k*58;
   char key[4]={'K',(char)('1'+k),0};cv_text(x,y+2,&FONT_XS,key,k==hk?MD_INK:MD_EDGE);cv_text(x+text_w(&FONT_XS,key)+4,y+2,&FONT_XS,value,k==hk?col:MD_DIM);}}
 if(touched){const char *label;char value[24],info[48];bd_knob(p,hk,&label,value,info,0);const int y=hk==1?42:164;
  char head[24]={'K',(char)('1'+hk),' ',0};ov_append(head,label);
  cv_rect(6,y,228,33,MD_CARD);cv_rect(6,y,228,1,col);cv_text(12,y+3,&FONT_XS,head,MD_DIM);
  cv_text(228-text_w(&FONT_B,value),y+2,&FONT_B,value,col);cv_text(12,y+18,&FONT_XS,info,MD_INK);}
}
