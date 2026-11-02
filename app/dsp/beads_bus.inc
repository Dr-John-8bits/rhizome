/* SPDX-License-Identifier: GPL-3.0-only
 * Rhizome: Beads (Mutable Instruments, MIT) as the granular bus, in place of Monades' grains. Included by dreams.cpp.
 * The tracks' grain sends go in (mono: twice the memory), Beads' grains come out, the SPACE macro sets their level.
 * Its 128 KiB are those of Monades' grain tape. The media choose the memory's grain and length: DIGITAL (16 bits,
 * 1.4 s), CLEAN TAPE (12 bits at half rate, 3.8 s), DIRTY TAPE (mu-law at half rate, with
 * wow and flutter, 5.8 s). The knobs keep Monades' fields where they mean the same: POSITION is Beads' TIME, GRAIN SIZE
 * its SIZE (DIRECTION reverse uses Beads' reversed third), PITCH, FEEDBACK, FREEZE (the comet's too); new: DENSITY (below
 * the middle, regular grains; above, random ones; the middle, none), SHAPE, RANDOM, MEDIA. */
enum { BEADS_BYTES=131072, BEADS_CHUNK=32 };
static uint8_t beads_mem[BEADS_BYTES] POOL;
alignas(beads::Processor) static unsigned char beads_raw[sizeof(beads::Processor)] POOL;
static beads::Processor *beads_proc;
static int beads_media_now=-1;
static uint8_t tape_peak[256];static unsigned tape_peak_col;
// Built once, at md_init (before the audio runs). Afterwards a reset (a Monde loaded from the screen's side, PANIC) only
// empties the memory and asks the audio thread to reconfigure Beads on its next block: the object is never rebuilt
// while the audio thread may be using it.
static void beads_reset(){memset(beads_mem,0,sizeof beads_mem);memset(tape_peak,0,sizeof tape_peak);tape_peak_col=0;beads_media_now=-1;}
static void beads_init(){
 beads_proc=new(beads_raw) beads::Processor();uint8_t *banks[2]={beads_mem,beads_mem+BEADS_BYTES/2};beads_proc->Init(banks);beads_reset();
}
static beads::Processor::Media beads_media(){static const beads::Processor::Media m[MD_BEADS_MEDIAS]={beads::Processor::MEDIA_DIRTY_TAPE,beads::Processor::MEDIA_CLEAN_TAPE,beads::Processor::MEDIA_DIGITAL};return m[bound(patch.beads_media,0,MD_BEADS_MEDIAS-1)];}
static int beads_line_length(){return beads_proc->max_delay_line_length();} // samples of the mono line in use
// One block: the grain sends in, Beads' grains out (wet only, at Beads' own level).
static void beads_block(const float *send,unsigned n,float *wl,float *wr,bool frozen){
 // SPACE at 0: the grains would not be heard, Beads rests (it records nothing meanwhile).
 if(!patch.macro[3]&&smooth[3]<.0005f){for(unsigned k=0;k<n;k++)wl[k]=wr[k]=0;return;}
 if(beads_media_now!=patch.beads_media){beads_media_now=patch.beads_media;beads_proc->Configure(beads_media(),true);memset(tape_peak,0,sizeof tape_peak);}
 beads::Parameters p;const float rnd=patch.beads_random*.001f;
 const float size=clamp01(unit(patch.grain_size)+lfo_grain_size*.5f);
 p.time.base=clamp01(unit(patch.grain_pos)+lfo_grain_pos*.5f);p.time.attenuverter=rnd*.5f;
 p.size.base=patch.reverse?(1-size)*.33f:.3334f+.63f*size;p.size.attenuverter=rnd*.25f; // reversed in the lower third; never Beads' delay mode
 p.shape.base=patch.beads_shape*.001f;p.shape.attenuverter=rnd*.4f;
 p.pitch.base=(float)patch.grain_pitch;p.pitch.attenuverter=rnd*.15f;
 p.time.has_cv=p.size.has_cv=p.shape.has_cv=p.pitch.has_cv=false;p.time.cv=p.size.cv=p.shape.cv=p.pitch.cv=0;p.monophonic_mode=false;
 beads::Processor::MixParameters m;m.feedback=unit(patch.feedback)*.9f;m.dry_wet=1;m.reverb_amount=0;m.output_gate=false;
 beads::ModulatedParameter d;d.base=patch.beads_density*.001f;d.cv=0;
 const int len=beads_line_length();
 for(unsigned o=0;o<n;o+=BEADS_CHUNK){const unsigned c=n-o<BEADS_CHUNK?n-o:BEADS_CHUNK;
  static beads::FloatFrame in[BEADS_CHUNK],out[BEADS_CHUNK];static bool fr[BEADS_CHUNK]; // off the audio stack (7.9 KB on the FM-1)
  for(unsigned i=0;i<c;i++){const float x=clip(send[o+i]);in[i].l=in[i].r=x;fr[i]=frozen;}
  beads_proc->Process(beads::SEED_BUTTON_STATE_LOCKED,nullptr,fr,d,p,m,in,out,c); // SEED latched, as on the module: grains all along
  for(unsigned i=0;i<c;i++){wl[o+i]=out[i].l;wr[o+i]=out[i].r;}
  // The overview of what is in memory: the input's level where the write head is.
  if(!frozen){const unsigned col=(unsigned)((float)*beads_proc->mutable_delay_write_ptr()*256.0f/(float)(len>0?len:1))&255;uint8_t a=0; // no 64-bit multiply (bsp/docs/CPU.md)
   for(unsigned i=0;i<c;i++){const uint8_t v=(uint8_t)fminf(255,fabsf(in[i].l)*254.9f);if(v>a)a=v;}
   if(col!=tape_peak_col){tape_peak_col=col;tape_peak[col]=0;}if(a>tape_peak[col])tape_peak[col]=a;stats.grain_writes+=c;}}
}
