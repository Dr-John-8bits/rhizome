/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: the TAPE ECHO and TAPE ECHO / AGE pages. Included by app.c after orbit_page.c (its drawing helpers).
 * The machine as the engine runs it: two reels turning with the tape's real speed (scaled down so the three spokes
 * never seem to turn backwards), the tension arm swinging with the wow, the tape along the heads, and on it what the
 * record head wrote, travelling toward the playback heads: an echo is seen before it is heard. Heads in use are lit,
 * white as they play back; each shows its time. FREEZE lifts the record head; STOP stops the reels. */
enum { TP_LX=64, TP_RX=176, TP_RY=86, TP_REEL=30, TP_Y=150, TP_REC=56, TP_RUN=132, TP_IN=36, TP_OUT=204 };
static md_reel tp;
static float tp_angle;
static uint32_t tp_ms;
static void tape_view_update(void){
 const uint32_t now=plat_ms(),dt=tp_ms?now-tp_ms:0;tp_ms=now;md_reel_view(&tp);
 // Revolutions a second: 0.2 at the slowest tape, 0.3 at 1, about 1.4 at the fastest (under a sixth of a turn a frame).
 if(dream_prefs.motion&&dt<200){const float v=fmaxf(tp.speed,0),r=fm_sqrtf(v);tp_angle-=dt*.001f*6.2831853f*.3f*r*fm_sqrtf(fm_sqrtf(r));} // speed^0.625
 if(tp_angle<-6.2831853f)tp_angle+=6.2831853f;
}
static void tp_fmt_ms(float ms,char *s){
 if(ms<1000){md_num((int)(ms+.5f),s);ov_append(s," MS");return;}
 const int c=(int)(ms/10+.5f);md_num(c/100,s);ov_append(s,".");char b[4]={(char)('0'+c/10%10),(char)('0'+c%10),0};ov_append(s,b);ov_append(s," S");
}
// The time of head k (1..3) at the tape's speed now.
static float tp_head_ms(int k){return tp.speed>.01f?tp.path*k/3/(.5f*tp.speed*44.1f):0;}
static const char *const tp_heads_names[8]={"","1","2","1+2","3","1+3","2+3","1+2+3"};
static void tp_knob(const md_patch *p,int page,int k,const char **label,char *value,char *info,int brief){
 info[0]=0;value[0]=0;const float ms=md_echo_ms();
 if(page==TAPE_ECHO){
  if(k==0){*label="TIME";if(p->echo_sync){ov_append(value,md_echo_sync_name(p->echo_sync));ov_append(info,"SYNCED: ");}
   if(!p->echo_sync||!brief){char b[16];tp_fmt_ms(ms,b);if(p->echo_sync)ov_append(info,b);else ov_append(value,b);}
   if(!brief){char b[8];const float sp=31500.0f*((p->tape_heads&4)?3:(p->tape_heads&2)?2:1)/3/(ms*22.05f);md_num((int)sp,b);
    ov_append(info,p->echo_sync?", TAPE X":"TAPE SPEED X");ov_append(info,b);b[0]='.';b[1]=(char)('0'+(int)(sp*10)%10);b[2]=0;ov_append(info,b);}}
  else if(k==1){*label="FEEDBACK";md_num(p->delay_fb/10,value);ov_append(info,p->delay_fb<300?"FEW REPEATS":p->delay_fb<600?"REPEATS FADE SLOWLY":"LONG TRAILS, DARKER EACH PASS");}
  else if(k==2){*label="HEADS";ov_append(value,tp_heads_names[p->tape_heads&7]);ov_append(info,"ECHOES AT ");
   static const char *const part[3]={"1/3","2/3","ALL"};int first=1;const int lng=(p->tape_heads&4)?3:(p->tape_heads&2)?2:1;
   for(int h=0;h<3;h++)if(p->tape_heads>>h&1){if(!first)ov_append(info,", ");first=0;
    if(lng==3)ov_append(info,part[h]);else{char b[8];tp_fmt_ms(ms*(h+1)/lng,b);ov_append(info,b);}}
   if(lng==3)ov_append(info," OF THE TIME");}
  else{*label="WOW";md_num(p->tape_wow/10,value);ov_append(info,p->tape_wow?"WOW, FLUTTER, DRIFT":"A STEADY TAPE");}
 }else{
  if(k==0){*label="DRIVE";md_num(p->tape_drive/10,value);ov_append(info,p->tape_drive?"THE RECORD HEAD SATURATES":"CLEAN RECORDING");}
  else if(k==1){*label="AGE";md_num(p->tape_age/10,value);ov_append(info,p->tape_age?"HISS, DULLER, DROPOUTS":"A NEW TAPE");}
  else if(k==2){*label="TONE";md_num(p->echo_tone/10,value);ov_append(info,p->echo_tone<400?"EACH PASS MUCH DARKER":p->echo_tone>600?"BRIGHT REPEATS":"EACH PASS A LITTLE DARKER");}
  else{static const char *const names[MD_TAPE_TRANSPORTS]={"RUN","FREEZE","STOP"},*const infos[MD_TAPE_TRANSPORTS]={"THE TAPE RUNS","REC HEAD UP: THE LOOP REPEATS","THE TAPE STOPS, THE SOUND DIVES"};
   *label="TRANSPORT";ov_append(value,names[tp.transport]);ov_append(info,infos[tp.transport]);}
 }
}
static void tp_reel(int cx,uint16_t col,int hk_speed){
 ov_ring(cx,TP_RY,TP_REEL,md_tint(MD_BG,MD_EDGE,24),1.2f);ov_ring(cx,TP_RY,TP_REEL-7,md_tint(MD_BG,col,hk_speed?20u:12u),1.2f);ov_ring(cx,TP_RY,TP_REEL-8,md_tint(MD_BG,col,hk_speed?16u:9u),1.2f);
 for(int k=0;k<3;k++){const float a=tp_angle+k*2.0943951f,c=fm_cosf(a),s=fm_sinf(a);ov_seg(cx+6*c,TP_RY+6*s,cx+(TP_REEL-10)*c,TP_RY+(TP_REEL-10)*s,md_tint(MD_BG,MD_INK,20));}
 ov_ring(cx,TP_RY,4,md_tint(MD_BG,MD_INK,22),1);
}
static void md_reel_page(const md_patch *p,uint16_t col){
 const int page=dream_ui.page;const uint32_t now=plat_ms();
 const int touched=ov_touch>=0&&ov_touch_page==page&&now-ov_touch_ms<1500,hk=touched?ov_touch:-1;
 const int heads=tp.heads,freeze=tp.transport==MD_TAPE_FREEZE;
 // Reels, and the tape from the left one down, along the heads, up to the right one.
 tp_reel(TP_LX,col,page==TAPE_ECHO&&hk==0);tp_reel(TP_RX,col,page==TAPE_ECHO&&hk==0);
 const uint16_t tape_ink=md_tint(MD_BG,MD_EDGE,26);
 ov_seg(TP_LX-TP_REEL+6,TP_RY,TP_IN,TP_Y,tape_ink);ov_seg(TP_IN,TP_Y,TP_OUT,TP_Y,tape_ink);ov_seg(TP_OUT,TP_Y,TP_RX+TP_REEL-6,TP_RY,tape_ink);
 ov_ring(TP_IN,TP_Y,3,MD_EDGE,1.2f);ov_ring(TP_OUT,TP_Y,3,MD_EDGE,1.2f);
 // The tension arm swings with the wow (the flutter is too fast to show: its depth is in the swing).
 {const float a=-.25f+.35f*tp.wow,x=20+15*fm_cosf(a),y=116+15*fm_sinf(a);ov_seg(20,116,x,y,md_tint(MD_BG,MD_INK,page==TAPE_ECHO&&hk==3?28u:14u));
  ov_ring(x,y,2.5f,md_tint(MD_BG,MD_INK,18),1);ov_ring(20,116,1.5f,MD_EDGE,1);}
 // What is on the tape: the level written on each stretch, from the record head to the right roller (older beyond
 // head 3), fainter before the record head, where the tape comes back round to be written over (kept under FREEZE).
 for(int x=TP_IN+4;x<TP_OUT-4;x++){
  const float behind=(x-TP_REC)*tp.path/TP_RUN;const int idx=((int)tp.pos-(int)behind)&(int)(tp.length-1);
  const float lv=tp.trace[idx>>8]*(1/255.0f);if(lv<.02f)continue;
  const unsigned bright=x<TP_REC?(freeze?20u:7u):x>TP_REC+TP_RUN?12u:(unsigned)(14+14*fminf(1,lv*1.5f));
  ov_seg((float)x,TP_Y-2,(float)x,TP_Y-2-24*fm_sqrtf(fminf(1,lv)),md_tint(MD_BG,col,bright));} // heights like a meter
 // Heads: the record head (lifted under FREEZE), then the three playback heads at their places on the path.
 {const int y=freeze?TP_Y+6:TP_Y+2;const uint16_t ink=freeze?MD_EDGE:tp.rec_peak>.05f?MD_INK:md_tint(MD_BG,col,26);
  cv_rect(TP_REC-4,y,9,10,ink);ov_label(TP_REC,TP_Y+24,freeze?"UP":"REC",freeze?MD_DIM:MD_EDGE);}
 for(int k=0;k<3;k++){const int x=TP_REC+TP_RUN*(k+1)/3,on=heads>>k&1;const float pk=tp.head_peak[k];
  if(on)cv_rect(x-4,TP_Y+2,9,10,pk>.04f?MD_INK:md_tint(MD_BG,col,(unsigned)(18+10*fminf(1,pk*20))));
  else{ov_seg(x-4,TP_Y+2,x-4,TP_Y+11,MD_EDGE);ov_seg(x+4,TP_Y+2,x+4,TP_Y+11,MD_EDGE);ov_seg(x-4,TP_Y+11,x+4,TP_Y+11,MD_EDGE);}
  char s[12];if(on)tp_fmt_ms(tp_head_ms(k+1),s);else{s[0]=(char)('1'+k);s[1]=0;}
  ov_label(x,TP_Y+24,s,on?(page==TAPE_ECHO&&(hk==0||hk==2)?col:MD_DIM):MD_EDGE);}
 // Words only where they help.
 if(!p->fx_level)md_text(10,44,"RETURN AT 0: FX > SPACE, K4",MD_DIM);
 else{int sends=0;for(int i=0;i<6;i++)sends+=p->ext[i].delay_send;if(!sends)md_text(10,44,"NO SEND: EDIT > MIXER, K3",MD_DIM);
  else if(tp.transport==MD_TAPE_STOP&&page==TAPE_ECHO)md_text(10,44,"STOPPED: K4 TRANSPORT ON AGE",MD_DIM);}
 // The strip, and the card of the knob being turned (over the reels, or over the heads for WOW and TRANSPORT).
 {const int y=200;cv_rect(0,y-1,240,25,MD_BG);cv_rect(6,y-1,228,1,MD_EDGE);
  for(int k=0;k<4;k++){const char *label;char value[24],info[48];tp_knob(p,page,k,&label,value,info,1);const int x=6+k*58;
   char key[4]={'K',(char)('1'+k),0};cv_text(x,y+2,&FONT_XS,key,k==hk?MD_INK:MD_EDGE);cv_text(x+text_w(&FONT_XS,key)+4,y+2,&FONT_XS,value,k==hk?col:MD_DIM);}}
 if(touched){const char *label;char value[24],info[48];tp_knob(p,page,hk,&label,value,info,0);const int y=hk==3?164:42;
  char head[24]={'K',(char)('1'+hk),' ',0};ov_append(head,label);
  cv_rect(6,y,228,33,MD_CARD);cv_rect(6,y,228,1,col);cv_text(12,y+3,&FONT_XS,head,MD_DIM);
  cv_text(228-text_w(&FONT_B,value),y+2,&FONT_B,value,col);cv_text(12,y+18,&FONT_XS,info,MD_INK);}
}
