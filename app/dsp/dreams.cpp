/* SPDX-License-Identifier: MIT OR GPL-3.0-only
 * Mutable Dreams. Original Mutable Instruments DSP remains under its MIT notices.
 * Six interchangeable monophonic slots. No allocations in render. */
#include "dreams.h"
extern "C" { __attribute__((weak)) uint32_t plat_ticks(void){return 0;} } // profiling (plat.h); the DSP alone (tests) has none
static const float PLAT_TICKS=24.0f; // plat.h PLAT_TICKS_PER_US
#include "elements/dsp/exciter.h"
#include "elements/dsp/resonator.h"
// Rhizome's two own voices (Plaits): SIX-OP FM and STRING MACHINE; the other twelve are Monades'
#include "plaits/dsp/engine2/string_machine_engine.h"
#include "plaits/dsp/engine2/six_op_engine.h"
#include "plaits/resources.h"
#include <math.h>
#include <string.h>
#include <new>
#include <stddef.h>
#include "plaits/dsp/engine/fm_engine.h"
#include "plaits/dsp/engine/virtual_analog_engine.h"
#include "plaits/dsp/drums/analog_bass_drum.h"
#include "plaits/dsp/drums/analog_snare_drum.h"
#include "plaits/dsp/drums/hi_hat.h"
#include "rings/dsp/resonator.h"
#include "plaits/dsp/engine/additive_engine.h"
#include "plaits/dsp/oscillator/sine_oscillator.h"
#include "stmlib/dsp/filter.h"
#include "stmlib/utils/random.h"
// The Rings chorus lives in chorus.cpp: its resources.h redefines Plaits' LUT_* macros.
extern "C" { void md_chorus_reset(uint16_t *line); void md_chorus_process(float *left,float *right,unsigned n,float amount,float depth); }
#ifndef PLAT_HOST
#define POOL __attribute__((section(".pool")))
#else
#define POOL
#endif
namespace {
#include "satin_tables.inc"
#include "scale_table.inc"
constexpr unsigned Q=64;
// Rhizome: SIX-OP FM and STRING MACHINE are built in each voice's arena (the engine, then the RAM it asks for), as large
// as the hungrier of the two; SIX-OP's DX7 bank is shared (sixop_bank).
template<class E,unsigned RAM> struct EngineSize { enum { bytes=(sizeof(E)+15)/16*16+RAM }; };
constexpr unsigned cmax(unsigned a,unsigned b){return a>b?a:b;}
enum : unsigned { ARENA=cmax(EngineSize<plaits::StringMachineEngine,1024*4>::bytes,EngineSize<plaits::SixOpEngine,plaits::kMaxBlockSize*(4+plaits::kNumSixOpVoices)*4>::bytes)+64 };
static size_t arena_spare=ARENA; // the least room an engine left in its arena (tests: never 0)
struct Voice {
 alignas(16) unsigned char arena[ARENA]; plaits::Engine *engine; int half; // Rhizome: SIX-OP or STRING MACHINE, built here; half: SIX-OP built for half rate (ECO)
 plaits::AdditiveEngine additive; plaits::SineOscillator choir[3]; stmlib::Svf filter1,filter2;
 plaits::FMEngine fm; plaits::VirtualAnalogEngine va;
 plaits::AnalogBassDrum kick; plaits::AnalogSnareDrum snare;
 plaits::HiHat<plaits::SquareNoise,plaits::SwingVCA,true,false> hat;
 rings::Resonator rings;
 float satin_morph,satin_tone;
 float expression_smooth,pan_smooth,midi_gate,mellow_phase,mellow_drift;
 float strike_gate,target_note,sub_phase,tide_phase,warp_phase,warp_smooth,delay_send_smooth,reverb_send_smooth;
 float mem[112],env,amp,note,excite,fade,velocity,cut_smooth,res_smooth,breath_phase,lpg,eco_previous,eco_next;
 int model,trigger,attacking,active,eco_phase;
 float orn_from,orn_age; int orn_style; // Monades gamaka: start offset, seconds since the note, style
};
alignas(Voice) static unsigned char voices_raw[sizeof(Voice)*6] POOL;
static Voice *voices;
// SIX-OP's DX7 sounds, decoded once at start for all six voices: Plaits' three banks, 96 patches.
static plaits::fm::Patch sixop_bank[96] POOL;
// Strike resonators use ordinary RAM; the large tape/effect pool stays bounded.
static elements::Resonator element_body[6];
static elements::Exciter element_strike[6];
static void reel_reset(); // Rhizome's tape echo (tape_echo.inc) holds the echo's 32 KiB
static bool fx_active;
static float fx_smooth,fx_highpass_in,fx_highpass_out;
static void hall_reset(); // Rhizome's HALL (hall.inc)
static void beads_reset(),beads_init(); // Rhizome's granular bus (beads_bus.inc)
static stmlib::Svf comet_lp[2];static bool comet_filtering; // Rhizome: the comet's FILTER (orbit.inc's comet_open)
static stmlib::Svf send_hp_filter[2];static int send_hp_set=-1; // the knob value the coefficients were computed for
// Monades: Rings chorus on the dry mix (4 KiB line), and Dattorro / Griesinger input diffusion before the room
// (four allpasses, 1210 samples). Both are skipped entirely at 0.
static uint16_t chorus_line[2048]; // ordinary RAM: the pool must keep 8 KB spare (base/tools/build.py)
// Monades: 12 dB high-pass on the echo and room sends
static void reset_fx(){send_hp_filter[0].Init();send_hp_filter[1].Init();memset(chorus_line,0,sizeof chorus_line);md_chorus_reset(chorus_line);send_hp_set=-1;fx_active=false;reel_reset();hall_reset();fx_smooth=fx_highpass_in=fx_highpass_out=0;}
// Rhizome v13: Monades' twelve voices again, and Rhizome's SIX-OP FM and STRING MACHINE. A v12 patch (Rhizome's eight
// Plaits voices) gets the nearest for each track, and no library sound; a v11 patch had Monades' already.
static void rhizome_v12_voices(md_patch &p){
 static const int8_t nearest[8]={MD_STRINGS,MD_SATIN,MD_MELLOW,MD_SIXOP,MD_VA,MD_FM,MD_CHOIR,MD_HARMONIC}; // STRINGS, WAVETABLE, TERRAIN, SIX-OP, SWARM, PD, CHORDS, FORMANT
 for(int i=0;i<6;i++){if(p.track[i].model>=0&&p.track[i].model<8)p.track[i].model=nearest[p.track[i].model];else p.track[i].model=-1;p.tone_preset[i]=-1;} // -1: refused by md_validate
}
static void v11_defaults(md_patch &p){p.version=13;for(int i=0;i<6;i++)p.orbit[i]={0,4,0,0,MD_PITCH_ORBIT,0,1,MD_MOON_UP,60};p.orbit_prec=p.orbit_pert=0;p.orbit_comet=0;p.comet_event=MD_COMET_SWELL;p.comet_amount=60;p.orbit_conj=MD_CONJ_ACCENT;p.orbit_system=0;p.tape_heads=1;p.tape_wow=p.tape_drive=p.tape_age=0;p.hall_size=700;p.hall_predelay=20;p.hall_bass=550;p.hall_mod=300;p.hall_vintage=0;p.beads_shape=500;p.beads_density=800;p.beads_random=200;p.beads_media=MD_BEADS_DIGITAL;} // Beads: frequent random grains // the echo: head 1 alone (40 ms to 0.95 s), a clean tape // Rhizome: orbits off, one bar a turn
static void v10_defaults(md_patch &p){p.motion_every=0;v11_defaults(p);}
static void v9_defaults(md_patch &p){for(int i=0;i<6;i++){p.gamaka[i]=MD_GAMAKA_OFF;p.gamaka_amount[i]=500;}p.vadi=p.samvadi=-1;v10_defaults(p);}
static void v8_defaults(md_patch &p){for(int i=0;i<6;i++)p.gen[i]={MD_GEN_OFF,500,8,12};p.form=0;p.form_bars=1;p.form_drops=2;v9_defaults(p);}
static void v7_defaults(md_patch &p){p.send_hp=0;p.echo_tone=p.room_tone=500;p.echo_sync=p.room_diffusion=p.chorus=0;p.chorus_depth=500;v8_defaults(p);for(int i=0;i<6;i++)for(int s=0;s<2;s++)p.lfo[i][s]={3,MD_LFO_SINE,s?MD_TO_PAN:MD_TO_CUTOFF,0};}
static void extension_defaults(md_patch &p){v7_defaults(p);p.delay_ms=200;p.delay_fb=300;p.reverb_time=450;p.fx_level=0;memset(p.ext,0,sizeof p.ext);for(int i=0;i<6;i++){p.ext[i].tides_rate=250;for(int j=0;j<16;j++)p.ext[i].notes[j]=(j%4==0?0:j%4==2?7:-1);}}

struct Repeat { float left,spacing,note,velocity; unsigned remaining; };
static Repeat repeats[6];
static md_patch patch,previous;
static float shock_burst; // a WANDER shock's jolt on the effects (orbit.inc), fading in about 1.5 s
#include "tape_echo.inc"
static md_stats stats;
// Display-only scope per voice: 128 points after the filter, stride chosen so about two periods fit 64 points.
static int16_t scope_buf[6][128];static unsigned scope_pos[6],scope_phase[6],scope_track; // only the track the screen reads is recorded; // display-only overview of the tape, 512 samples a column
static float dcx[2],dcy[2],smooth[4];
static unsigned rng,seqrng,step;
static float untilstep,duckenv;
static int playing;
static int clock_source,midi_quantize,clock_pulse;
static unsigned clock_age,clock_last;static float clock_interval;
static int held[6],sustain[6],midi_bank[6];
static float bend[6],bend_smooth[6],expression[6];
struct Command { int c,t,v; };
static Command queue[64];
static volatile unsigned qread,qwrite;
static inline void fence() { __asm__ volatile("" ::: "memory"); }
static int bound(int x,int a,int b) { return x<a?a:x>b?b:x; }
static float unit(int x) { return bound(x,0,1000)*.001f; }
static uint16_t custom_scale=0x091; /* CUSTOM: root, major third, fifth until the player edits it */
static int last_generated[6]; /* per track, for ascent / descent sets */
static int motion_held[6]; /* Motion's degree, held for motion_every steps */
// Ascending into a note uses the ascent set, descending the descent set (ragas such as Desh or Jaunpuri, classical melodic minor).
static unsigned micro_mask(const MicroDef &m){unsigned mask=1;for(unsigned k=0;k<m.count;k++)mask|=1u<<((int)(m.degree[k]/100.0f+.5f)%12);return mask;} /* nearest semitones, for twelve-tone views */
static unsigned scale_mask_for(int scale,int down){scale=bound(scale,0,SCALE_COUNT-1);if(scale==SCALE_CUSTOM)return custom_scale|1u;if(scale>=SCALE_TWELVE)return micro_mask(micro_defs[scale-SCALE_TWELVE]);return down?scale_defs[scale].down:scale_defs[scale].up;}
// Microtonal: the nearest degree in cents (ties go down), repeated every period above the root; a fractional MIDI note.
static float quantize_micro(float note,int root,const MicroDef &m){
 note=fminf(96.0f,fmaxf(24.0f,note));root=bound(root,0,11);
 float rel=(note-root)*100.0f,k=floorf(rel/m.period),r=rel-k*m.period,best=0,dist=r;
 for(unsigned j=0;j<=m.count;j++){float d=j<m.count?m.degree[j]:m.period,e=fabsf(r-d);if(e<dist){dist=e;best=d;}}
 float out=root+(k*m.period+best)*.01f;
 while(out>96.0f)out-=m.period*.01f;while(out<24.0f)out+=m.period*.01f;
 return out;
}
static int quantize_mask(int note,int root,unsigned mask){
 note=bound(note,24,96);root=bound(root,0,11);
 for(int delta=0;delta<12;delta++){
  int low=note-delta,high=note+delta;
  if(low>=24&&(mask&(1u<<((low-root+12)%12))))return low;
  if(high<=96&&(mask&(1u<<((high-root+12)%12))))return high;
 }return note;
}
static float quantize_any(float note,int root,int scale,int down){
 scale=bound(scale,0,SCALE_COUNT-1);
 if(scale>=SCALE_TWELVE)return quantize_micro(note,root,micro_defs[scale-SCALE_TWELVE]);
 return (float)quantize_mask((int)floorf(note+.5f),root,scale_mask_for(scale,down));
}
static unsigned random_word(unsigned &r) { r^=r<<13;r^=r>>17;r^=r<<5;return r; }
static float noise() { return (random_word(rng)>>8)*(1.0f/8388608.0f)-1.0f; }
// Linear at ordinary levels; only excess peaks enter the soft safety knee.
static float clip(float x) { float a=fabsf(x);if(a<=.8f)return x;float y=.8f+.2f*(a-.8f)/(.2f+a-.8f);return x<0?-y:y; }
// Keep existing warm settings up to 90%; open the final tenth to 18 kHz.
static float filter_q(float resonance) { return .707f+2.0f*resonance*resonance; }
static float cutoff_hz(float u) { return 60.0f*stmlib::SemitonesToRatio(u*84.0f+fmaxf(0.0f,u-.9f)*147.458f); }
// Rhizome: SIX-OP or STRING MACHINE, built in the voice's arena; the rest of the arena is the RAM it asks for.
template<class E> static plaits::Engine *build(Voice &v){E *e=new (v.arena) E();const unsigned at=(sizeof(E)+15)/16*16;
 stmlib::BufferAllocator a(v.arena+at,ARENA-at);e->Init(&a);e->Reset();arena_spare=a.free()<arena_spare?a.free():arena_spare;return e;}
static void build_engine(Voice &v,int model){
 v.engine=nullptr;
 if(model==MD_STRINGS)v.engine=build<plaits::StringMachineEngine>(v);
 else if(model==MD_SIXOP){plaits::SixOpEngine *e=static_cast<plaits::SixOpEngine*>(build<plaits::SixOpEngine>(v));v.half=patch.quality==0;e->set_half_rate(v.half);e->set_patches(sixop_bank,96);v.engine=e;}
}
static void reset_voice(unsigned i) {
 memset(static_cast<void*>(&voices[i]),0,sizeof(Voice));
 Voice *v=new (&voices[i]) Voice();
 stmlib::BufferAllocator a(v->mem,sizeof v->mem);
 v->fm.Init(&a); v->va.Init(&a);v->kick.Init();v->snare.Init();v->hat.Init();v->rings.Init();
 v->additive.Init(&a);v->additive.Reset();build_engine(*v,patch.track[i].model);v->filter1.Init();v->filter2.Init();for(int k=0;k<3;k++)v->choir[k].Init();
 v->cut_smooth=patch.track[i].cutoff*.001f;v->res_smooth=patch.track[i].resonance*.001f;v->breath_phase=i*.137f;
 v->expression_smooth=1;v->pan_smooth=unit(patch.track[i].pan);held[i]=-1;sustain[i]=0;bend[i]=bend_smooth[i]=0;expression[i]=1;
 v->model=patch.track[i].model;v->note=(float)patch.track[i].note;v->target_note=v->note;v->fade=0;v->velocity=.8f;
 memset(static_cast<void*>(&element_body[i]),0,sizeof(element_body[i]));element_body[i].Init();element_body[i].set_modulation_frequency(.5f/MD_RATE);element_body[i].set_modulation_offset(.25f);
 memset(static_cast<void*>(&element_strike[i]),0,sizeof(element_strike[i]));element_strike[i].Init();
 repeats[i].remaining=0;
}
#include "preset_bank.inc"
static void clear_audio() {
 reset_fx();beads_reset();
 memset(repeats,0,sizeof repeats);
 memset(dcx,0,sizeof dcx);memset(dcy,0,sizeof dcy);duckenv=0;
}
static void gen_reset();
static int morph_on,drift_on,drift_slow,fx_shed; /* a morph is under way (morph.inc), driven by DERIVE (drift.inc); a new Monde ends both */
// A factory Monde into p, in place: the audio thread has no stack to spare for a copy. Rhizome's are complete v11 patches.
static void bank_patch(md_patch &p,int id) {memcpy(&p,&biome_bank[bound(id,0,MD_BIOMES-1)].patch,sizeof(md_patch));}
// Unsaved work (from Mutable Dreams 1.3): a fingerprint of the patch when it was loaded or saved. What changes by
// itself (the free seed, the auto ECO drop, the tape freeze, the edit flags, the phrase cursor, and the tempo while it
// follows a received clock) is left out.
static uint32_t saved_hash[2];static int32_t saved_tempo; /* the tempo apart: it counts only on the internal clock */
static void patch_hash(uint32_t out[2]){
 const uint8_t *b=reinterpret_cast<const uint8_t*>(&patch);uint32_t a=2166136261u,c=5381;
 const unsigned skip[4]={offsetof(md_patch,seed),offsetof(md_patch,freeze),offsetof(md_patch,quality),offsetof(md_patch,edited_mask)};
 const unsigned ext=offsetof(md_patch,ext),ext_end=ext+sizeof patch.ext,cursor=offsetof(md_extension,edit_step);
 for(unsigned j=0;j<sizeof patch;j++){
  bool out_of=false;for(unsigned k=0;k<4;k++)if(j>=skip[k]&&j<skip[k]+4)out_of=true;
  if(j>=offsetof(md_patch,tempo)&&j<offsetof(md_patch,tempo)+4)out_of=true;
  if(j>=ext&&j<ext_end){unsigned o=(j-ext)%sizeof(md_extension);if(o>=cursor&&o<cursor+4)out_of=true;}
  if(out_of)continue;a=(a^b[j])*16777619u;c=(c*33u)^b[j];
 }
 out[0]=a;out[1]=c;
}
static void mark_saved(){patch_hash(saved_hash);saved_tempo=patch.tempo;}
static void scene(int id) {
 bank_patch(patch,id);morph_on=0;drift_on=0;drift_slow=0;fx_shed=0;
 clear_audio();for(int i=0;i<6;i++){reset_voice(i);}for(int j=0;j<4;j++)smooth[j]=unit(patch.macro[j]);
 previous=patch;step=0;untilstep=0;seqrng=patch.seed;
 gen_reset();memset(motion_held,0,sizeof motion_held); // a new Biome starts with empty generator registers
 mark_saved();
}
static void tone_preset(int track,int id) {
 id=bound(id,0,MD_TONE_PRESETS-1);const TonePreset &a=tone_bank[id];md_track &t=patch.track[track];
 t.model=a.model;t.tone=a.tone;t.harm=a.harm;t.decay=a.decay;t.attack=a.attack;t.shape=a.shape;
 t.cutoff=a.cutoff;t.resonance=a.resonance;t.filter=a.filter;t.breathe=a.breathe;
 patch.tone_preset[track]=id;patch.edited_mask=(patch.edited_mask&~(2u<<track))|1u;reset_voice(track);
}
static int32_t *track_field(md_track &t,int c) {
switch(c){case MD_MODEL:return &t.model;case MD_TONE:return &t.tone;case MD_HARM:return &t.harm;case MD_DECAY:return &t.decay;case MD_LEVEL:return &t.level;case MD_SEND:return &t.send;case MD_HITS:return &t.hits;case MD_LENGTH:return &t.length;case MD_ROTATE:return &t.rotate;case MD_PROB:return &t.prob;case MD_NOTE:return &t.note;case MD_PAN:return &t.pan;case MD_MUTE:return &t.mute;default:return &t.speed;}
}
#include "midi_out.inc"
#include "lfo.inc"
static float clamp01(float x){return x<0?0:x>1?1:x;}
#include "generative.inc"
#include "morph.inc"
#include "drift.inc"
// Echo sync: the division in sixteenths.
static const float echo_sync_len[MD_ECHO_SYNCS]={0,.5f,2/3.0f,1,4/3.0f,1.5f,2,8/3.0f,3,4,6,8,12}; // sixteenths
static float echo_ms_now(){
 const float hi=reel_ms_max(),lo=reel_ms_min();
 if(!patch.echo_sync)return fminf(hi,fmaxf(lo,(float)patch.delay_ms)); // the tape's speed limits the time of the heads in use
 float ms=echo_sync_len[bound(patch.echo_sync,1,MD_ECHO_SYNCS-1)]*15000.0f/patch.tempo;
 while(ms>hi)ms*=.5f;while(ms<lo)ms*=2; // too long for the tape: half the division (still on the grid)
 return ms;
}
// Echo / room damping: 500 keeps Mutable Dreams' exact coefficient.
static float tone_coef(int v,float lo,float mid,float hi){return v==500?mid:v<500?lo+(mid-lo)*v*.002f:mid+(hi-mid)*(v-500)*.002f;}
static void fire(unsigned i,float note,float velocity) {
 Voice &v=voices[i];
 {int style=patch.gamaka[i];v.orn_style=0;v.orn_age=0;
  if(style&&patch.track[i].model>=MD_FM){
   if(style==MD_GAMAKA_MIX){unsigned r=patch.seed^(stats.events[i]*0x9e3779b9u)^(i*0x85ebca6bu);random_word(r);style=MD_GAMAKA_KAN+(int)(r%3);}
   float prev=stats.last_pitch[i]>0?stats.last_pitch[i]:note;
   v.orn_from=style==MD_GAMAKA_KAN?scale_neighbour(note,1)-note:style==MD_GAMAKA_ANDOLAN?scale_neighbour(note,-1)-note:fmaxf(-7.0f,fminf(7.0f,prev-note));
   v.orn_style=style;}
  stats.gamaka_style[i]=v.orn_style;}
 v.trigger=1;v.midi_gate=0;v.target_note=note;if(!patch.ext[i].glide||!v.active)v.note=note;v.velocity=velocity;v.active=1;
 stats.events[i]++;stats.last_note[i]=(int)note;stats.last_pitch[i]=note;
 if((int)i==patch.duck_source&&!patch.track[i].mute)duckenv=1;
}
static unsigned dice_rng=0x2545f491u; // its own generator: a dice roll leaves the sequencer's chance untouched
static int dice_in(int lo,int hi){return lo+(int)(random_word(dice_rng)%(unsigned)(hi-lo+1));}
// Dice: the page's own settings, each within a range that keeps the track audible and the effects tame (filter never
// shut, feedback far from runaway, pitch LFOs shallow). The effects pages also make sure the effects can be heard.
static void dice_audible_fx(int i){
 if(patch.fx_level<250)patch.fx_level=dice_in(400,700);
 int sends=0;for(int j=0;j<6;j++)sends+=patch.ext[j].delay_send+patch.ext[j].reverb_send;
 if(!sends){patch.ext[i].delay_send=dice_in(200,500);patch.ext[i].reverb_send=dice_in(200,500);}
}
static void dice(int i,int page){
 if(!drift_on)previous=patch; /* during DERIVE, ENV goes home */patch.edited_mask|=1u;dice_rng^=stats.frames|1u;md_track &t=patch.track[i];md_extension &x=patch.ext[i];
 switch(page){
 case MD_DICE_SOUND:t.tone=dice_in(0,1000);t.harm=dice_in(0,1000);if(md_has_envelope(t.model)||t.model==MD_ELEMENTS)t.shape=dice_in(0,1000);else t.decay=dice_in(150,900);break;
 case MD_DICE_FILTER:{static const int modes[]={1,1,2,2,3,4,5,6,7};t.filter=modes[dice_in(0,8)];t.resonance=dice_in(0,650);t.breathe=dice_in(0,600);
  t.cutoff=t.filter==4||t.filter==7?dice_in(0,450):t.filter==5?dice_in(300,800):dice_in(400,1000);break;} // high-pass low, band-pass in the middle
 case MD_DICE_ENVELOPE:if(md_has_envelope(t.model))t.attack=dice_in(0,350);t.decay=dice_in(150,900);t.send=dice_in(0,600);break;
 case MD_DICE_MIX:t.send=dice_in(0,600);t.pan=dice_in(150,850);t.speed=dice_in(1,4);break;
 case MD_DICE_MIXER:t.level=dice_in(300,800);t.send=dice_in(0,600);x.delay_send=dice_in(0,600);x.reverb_send=dice_in(0,600);break;
 case MD_DICE_RHYTHM:t.length=dice_in(5,16);t.hits=dice_in(1,t.length*3/4>1?t.length*3/4:1);t.rotate=dice_in(0,t.length-1);t.prob=dice_in(550,1000);break;
 case MD_DICE_GROOVE:patch.swing=dice_in(0,400);t.ratchet=dice_in(0,4)?1:2;t.accent=dice_in(0,800);t.speed=dice_in(1,4);break;
 case MD_DICE_BASS:x.pattern=dice_in(0,3);x.glide=dice_in(0,600);break;
 case MD_DICE_PHRASE:for(int k=0;k<16;k++)x.notes[k]=dice_in(0,9)<4?-1:dice_in(0,12);if(x.notes[0]<0)x.notes[0]=0;t.accent=dice_in(0,800);t.prob=dice_in(550,1000);break; // the first step sounds
 case MD_DICE_FORM:patch.form=dice_in(300,1000);patch.form_bars=dice_in(0,3);patch.form_drops=dice_in(0,3);patch.motion_every=dice_in(0,4);break;
 case MD_DICE_GRAIN:patch.macro[3]=dice_in(150,700);patch.grain_size=dice_in(100,900);patch.grain_pos=dice_in(0,1000);patch.feedback=dice_in(0,600);break;
 case MD_DICE_TAPE:{static const int pitch[]={-12,-7,-5,0,0,0,5,7,12};const int was_pitch=patch.grain_pitch,was_reverse=patch.reverse; // few choices: draw until one moves
  for(int tries=0;tries<8&&patch.grain_pitch==was_pitch&&patch.reverse==was_reverse;tries++){patch.grain_pitch=pitch[dice_in(0,8)];patch.reverse=dice_in(0,3)==0;}
  if(patch.duck_source>=0)patch.duck_amount=dice_in(0,600);break;}
 case MD_DICE_SPACE:if(patch.echo_sync)patch.echo_sync=dice_in(1,MD_ECHO_SYNCS-1);else patch.delay_ms=dice_in(80,350);patch.delay_fb=dice_in(0,600);patch.reverb_time=dice_in(200,850);patch.fx_level=dice_in(300,700);dice_audible_fx(i);break;
 case MD_DICE_SPACE_TONE:patch.send_hp=dice_in(0,500);patch.echo_tone=dice_in(200,800);patch.room_tone=dice_in(200,800);patch.room_diffusion=dice_in(0,1000);dice_audible_fx(i);break;
 case MD_DICE_ECHO:patch.echo_sync=dice_in(0,MD_ECHO_SYNCS-1);patch.delay_fb=dice_in(0,600);patch.chorus=dice_in(0,600);patch.chorus_depth=dice_in(0,1000);dice_audible_fx(i);break;
 case MD_DICE_MODULATION:x.marbles=dice_in(0,800);x.tides=dice_in(0,800);x.tides_rate=dice_in(0,1000);x.warps=dice_in(0,600);break;
 case MD_DICE_LFO_A:case MD_DICE_LFO_B:{md_lfo &l=patch.lfo[i][page==MD_DICE_LFO_B];l.rate=dice_in(0,20);l.shape=dice_in(0,MD_LFO_SHAPES-1);
  do l.target=dice_in(0,MD_LFO_TARGETS-1);while(!t.filter&&(l.target==MD_TO_CUTOFF||l.target==MD_TO_RESONANCE)); // the filter targets need a filter
  const int depth=l.target==MD_TO_PITCH?dice_in(50,300):dice_in(200,700);l.depth=dice_in(0,1)?depth:-depth;break;} // rate: synced, or free up to a few Hz
 case MD_DICE_RAGA:{const unsigned mask=scale_mask_for(patch.scale,0);int notes[12],n=0;for(int k=1;k<12;k++)if((mask>>k)&1)notes[n++]=k;
  patch.vadi=n?notes[dice_in(0,n-1)]:-1;patch.samvadi=dice_in(0,2)?0:n?notes[dice_in(0,n-1)]:-1;if(patch.samvadi==patch.vadi)patch.samvadi=0;
  patch.gamaka[i]=dice_in(0,MD_GAMAKAS-1);patch.gamaka_amount[i]=dice_in(200,900);break;}
 }
 if(page<=MD_DICE_ENVELOPE){patch.tone_preset[i]=-1;patch.edited_mask|=2u<<i;}
}
#include "orbit.inc"
#include "hall.inc"
#include "beads_bus.inc"
static void apply_command(Command e) {
 int i=bound(e.t,0,5),v=e.v;md_track &t=patch.track[i];
 if(e.c>=MD_ORBIT_GATES&&e.c<=MD_ORBIT_MOON_LEVEL&&e.c!=MD_ORBIT_PITCH)patch.orbit_system=0; // Rhizome: a real system, edited, is your own
 // Playback, preview notes and emergency silence do not change a preset.
 if(e.c!=MD_SCENE&&e.c!=MD_RETURN&&e.c!=MD_PLAY&&e.c!=MD_TRIGGER&&e.c!=MD_PANIC&&e.c!=MD_MUTATE&&e.c!=MD_DICE&&e.c!=MD_QUALITY&&(e.c<MD_MORPH_TO||e.c>MD_OVERLOAD))patch.edited_mask|=1u;
 if(e.c==MD_MODEL||e.c==MD_TONE||e.c==MD_HARM||e.c==MD_DECAY||e.c==MD_ATTACK||(e.c>=MD_SHAPE&&e.c<=MD_BREATHE))patch.edited_mask|=2u<<i;
 if(e.c<=MD_SPEED) {
  int32_t *p=track_field(t,e.c);
  if(e.c==MD_MODEL) { *p=bound(v,0,MD_MODELS-1);patch.tone_preset[i]=-1;reset_voice(i); }
  else if(e.c==MD_LENGTH) { t.length=bound(v,1,32);t.hits=bound(t.hits,0,t.length);t.rotate%=t.length; }
  else if(e.c==MD_HITS) t.hits=bound(v,0,t.length);
  else if(e.c==MD_ROTATE) t.rotate=bound(v,0,t.length-1);
  else if(e.c==MD_NOTE) t.note=bound(v,24,96);
  else if(e.c==MD_MUTE) t.mute=!!v;
  else if(e.c==MD_SPEED) t.speed=bound(v,1,8);
  else *p=bound(v,0,1000);
 } else switch(e.c) {
 case MD_MACRO:patch.macro[bound(e.t,0,3)]=bound(v,0,1000);break;
 case MD_TEMPO:patch.tempo=bound(v,30,220);break;
 case MD_PLAY:{int was=playing;playing=!!v;memset(repeats,0,sizeof repeats);if(playing){step=0;untilstep=0;seqrng=patch.seed;beat=0;beat_limit=1;beat_wraps=0;gen_reset();orbit_reset();memset(motion_held,0,sizeof motion_held);}
  if(routing.clock&&!clock_source&&playing!=was){out_push(playing?0xfa:0xfc,0,0);clock_left=0;}if(!playing)out_all_off();}break;
 case MD_LOCK:patch.locked=!!v;break;
 case MD_MUTATE: if(!drift_on)previous=patch;patch.edited_mask|=1u;patch.seed=random_word(seqrng)|1;for(unsigned j=0;j<6;j++)patch.track[j].rotate=random_word(seqrng)%patch.track[j].length;break;
 case MD_RETURN:patch=drift_on?drift_home:previous;seqrng=patch.seed;morph_back();drift_back();for(unsigned j=0;j<6;j++){repeats[j].remaining=0;if(voices[j].model!=patch.track[j].model)reset_voice(j);}break;
 case MD_SCENE:scene(v);break;
 case MD_DICE:dice(i,bound(v,0,MD_DICE_PAGES-1));break;
 case MD_TONE_PRESET:tone_preset(i,v);break;
 case MD_FREEZE:patch.freeze=!!v;break;
 case MD_GRAIN_SIZE:patch.grain_size=bound(v,0,1000);break;
 case MD_GRAIN_POS:patch.grain_pos=bound(v,0,1000);break;
 case MD_FEEDBACK:patch.feedback=bound(v,0,850);break;
 case MD_QUALITY:if(patch.quality!=bound(v,0,1)){patch.quality=bound(v,0,1);for(unsigned j=0;j<6;j++){voices[j].eco_phase=0;voices[j].eco_previous=voices[j].eco_next=0;voices[j].fade=0;}}break;
 case MD_OVERLOAD:stats.quality_reductions++;if(patch.quality){patch.quality=0;for(unsigned j=0;j<6;j++){voices[j].eco_phase=0;voices[j].eco_previous=voices[j].eco_next=0;voices[j].fade=0;}}else fx_shed=1;break; // already ECO: the chorus and diffusion go
 case MD_TRIGGER:v=bound(v,0,127);fire(i,t.model>=MD_FM?quantize_any((float)(v+patch.transpose),patch.root,patch.scale,0):(float)bound(v,24,96),.8f);break;
 case MD_ATTACK:t.attack=bound(v,0,1000);break;
 case MD_RATCHET:t.ratchet=bound(v,1,4);break;
 case MD_SHAPE:t.shape=bound(v,0,1000);break;
 case MD_CUTOFF:t.cutoff=bound(v,0,1000);break;
 case MD_RESONANCE:t.resonance=bound(v,0,1000);break;
 case MD_FILTER:if(t.filter!=bound(v,0,MD_FILTER_MODES-1)){v=bound(v,0,MD_FILTER_MODES-1);t.filter=v;voices[i].filter1.Reset();voices[i].filter2.Reset();voices[i].fade=0;}break;
 case MD_BREATHE:t.breathe=bound(v,0,1000);break;
 case MD_ACCENT:t.accent=bound(v,0,1000);break;
 case MD_SWING:patch.swing=bound(v,0,600);break;
 case MD_ORBIT_GATES:patch.orbit[i].gates=bound(v,0,MD_ORBIT_GATES_MAX);break;
 case MD_ORBIT_PERIOD:orbit_rebase_period(i,bound(v,0,MD_ORBIT_PERIODS-1));break;
 case MD_ORBIT_ECC:patch.orbit[i].ecc=bound(v,0,MD_ORBIT_ECC_MAX);break;
 case MD_ORBIT_PHASE:patch.orbit[i].phase=(v%MD_ORBIT_PHASES+MD_ORBIT_PHASES)%MD_ORBIT_PHASES;break;
 case MD_ORBIT_PITCH:patch.orbit[i].pitch=bound(v,0,MD_PITCH_SOURCES-1);break;
 case MD_ORBIT_PREC:orbit_rebase_prec(bound(v,0,100));break;
 case MD_ORBIT_PERT:patch.orbit_pert=bound(v,0,100);break;
 case MD_ORBIT_MOONS:patch.orbit[i].moons=bound(v,0,MD_MOONS_MAX);break;
 case MD_ORBIT_MOON_SPIN:patch.orbit[i].moon_spin=bound(v,0,MD_MOON_SPINS-1);break;
 case MD_ORBIT_MOON_INTERVAL:patch.orbit[i].moon_interval=bound(v,0,MD_MOON_INTERVALS-1);break;
 case MD_ORBIT_MOON_LEVEL:patch.orbit[i].moon_level=bound(v,0,100);break;
 case MD_ORBIT_COMET:orbit_rebase_comet(bound(v,0,MD_COMETS-1));break;
 case MD_ORBIT_COMET_EVENT:patch.comet_event=bound(v,0,MD_COMET_EVENTS-1);break;
 case MD_ORBIT_COMET_AMOUNT:patch.comet_amount=bound(v,0,100);break;
 case MD_ORBIT_CONJ:patch.orbit_conj=bound(v,0,MD_CONJ_MODES-1);break;
 case MD_ORBIT_SYSTEM:orbit_load_system(bound(v,0,MD_ORBIT_SYSTEMS-1));break;
 case MD_ROOT:patch.root=bound(v,0,11);break;
 case MD_SCALE:patch.scale=bound(v,0,SCALE_COUNT-1);break;
 case MD_TRANSPOSE:patch.transpose=bound(v,-24,24);break;
 case MD_GRAIN_PITCH:patch.grain_pitch=bound(v,-24,24);break;
 case MD_REVERSE:patch.reverse=!!v;break;
 case MD_DUCK_SOURCE:patch.duck_source=bound(v,-1,5);break;
 case MD_DUCK:patch.duck_amount=bound(v,0,1000);break;
 case MD_DUCK_RELEASE:patch.duck_release=bound(v,0,1000);break;
 case MD_DELAY_SEND:patch.ext[i].delay_send=bound(v,0,1000);break;
 case MD_REVERB_SEND:patch.ext[i].reverb_send=bound(v,0,1000);break;
 case MD_DELAY_TIME:patch.delay_ms=bound(v,40,MD_TAPE_MS_MAX);break;
 case MD_TAPE_HEADS:patch.tape_heads=bound(v,1,7);break;
 case MD_TAPE_WOW:patch.tape_wow=bound(v,0,1000);break;
 case MD_TAPE_DRIVE:patch.tape_drive=bound(v,0,1000);break;
 case MD_TAPE_AGE:patch.tape_age=bound(v,0,1000);break;
 case MD_TAPE_TRANSPORT:reel_transport=bound(v,0,MD_TAPE_TRANSPORTS-1);break;
 case MD_DELAY_FB:patch.delay_fb=bound(v,0,800);break;
 case MD_REVERB_TIME:patch.reverb_time=bound(v,0,1000);break;
 case MD_HALL_SIZE:patch.hall_size=bound(v,0,1000);break;
 case MD_HALL_PREDELAY:patch.hall_predelay=bound(v,0,250);break;
 case MD_HALL_BASS:patch.hall_bass=bound(v,0,1000);break;
 case MD_HALL_MOD:patch.hall_mod=bound(v,0,1000);break;
 case MD_HALL_VINTAGE:patch.hall_vintage=!!v;break;
 case MD_BEADS_SHAPE:patch.beads_shape=bound(v,0,1000);break;
 case MD_BEADS_DENSITY:patch.beads_density=bound(v,0,1000);break;
 case MD_BEADS_RANDOM:patch.beads_random=bound(v,0,1000);break;
 case MD_BEADS_MEDIA:patch.beads_media=bound(v,0,MD_BEADS_MEDIAS-1);break;
 case MD_FX_LEVEL:patch.fx_level=bound(v,0,1000);break;
 case MD_CLOCK_SOURCE: // leaving the internal clock stops the sequence: tell the MPC, release the notes out
  if(bound(v,0,2)==clock_source)break; // the same source (a knob at its end): nothing changes, nothing stops
  if(playing&&!clock_source&&routing.clock)out_push(0xfc,0,0);if(playing)out_all_off();
  clock_source=bound(v,0,2);playing=0;clock_pulse=0;clock_age=clock_last=0;clock_interval=0;break;
 case MD_MIDI_QUANTIZE:midi_quantize=!!v;break;
 case MD_BASS_MODE:patch.ext[i].bass_mode=bound(v,0,2);break;
 case MD_BASS_PATTERN:{static const int motifs[4][16]={{0,-1,0,-1,7,-1,0,-1,0,-1,3,-1,7,-1,10,-1},{0,0,-1,7,0,-1,3,-1,0,0,7,-1,10,-1,7,-1},{0,-1,-1,-1,0,-1,7,-1,3,-1,-1,-1,7,-1,5,-1},{0,-1,12,7,0,3,-1,7,0,-1,10,7,5,-1,3,-1}};patch.ext[i].pattern=bound(v,0,3);memcpy(patch.ext[i].notes,motifs[patch.ext[i].pattern],sizeof motifs[0]);break;}
 case MD_GLIDE:patch.ext[i].glide=bound(v,0,1000);break;
 case MD_MARBLES:patch.ext[i].marbles=bound(v,0,1000);break;
 case MD_TIDES:patch.ext[i].tides=bound(v,0,1000);break;
 case MD_TIDES_RATE:patch.ext[i].tides_rate=bound(v,0,1000);break;
 case MD_WARPS:patch.ext[i].warps=bound(v,0,1000);break;
 case MD_BASS_STEP:patch.ext[i].edit_step=bound(v,0,15);break;
 case MD_BASS_VALUE:patch.ext[i].notes[patch.ext[i].edit_step]=bound(v,-1,24);break;
 case MD_LFO_RATE:case MD_LFO_SHAPE:case MD_LFO_TARGET:case MD_LFO_DEPTH:{if(e.t<0||e.t>11)break; /* a malformed track: ignored, not LFO B of track 6 */md_lfo &l=patch.lfo[e.t%6][e.t/6];
  if(e.c==MD_LFO_RATE)l.rate=bound(v,0,MD_LFO_RATES-1);else if(e.c==MD_LFO_SHAPE)l.shape=bound(v,0,MD_LFO_SHAPES-1);else if(e.c==MD_LFO_TARGET)l.target=bound(v,0,MD_LFO_TARGETS-1);else l.depth=bound(v,-1000,1000);break;}
 case MD_SEND_HP:patch.send_hp=bound(v,0,1000);break;
 case MD_ECHO_TONE:patch.echo_tone=bound(v,0,1000);break;
 case MD_ROOM_TONE:patch.room_tone=bound(v,0,1000);break;
 case MD_ECHO_SYNC:patch.echo_sync=bound(v,0,MD_ECHO_SYNCS-1);break;
 case MD_ROOM_DIFFUSION:patch.room_diffusion=bound(v,0,1000);break;
 case MD_CHORUS:patch.chorus=bound(v,0,1000);break;
 case MD_CHORUS_DEPTH:patch.chorus_depth=bound(v,0,1000);break;
 case MD_GEN_SOURCE:patch.gen[i].source=bound(v,0,MD_GEN_SOURCES-1);break;
 case MD_GEN_DEJAVU:patch.gen[i].dejavu=bound(v,0,1000);break;
 case MD_GEN_LOOP:patch.gen[i].loop=bound(v,1,16);break;
 case MD_GEN_SPREAD:patch.gen[i].spread=bound(v,0,24);break;
 case MD_FORM:patch.form=bound(v,0,1000);break;
 case MD_FORM_BARS:patch.form_bars=bound(v,0,3);break;
 case MD_FORM_DROPS:patch.form_drops=bound(v,0,3);break;
 case MD_GAMAKA:patch.gamaka[i]=bound(v,0,MD_GAMAKAS-1);break;
 case MD_GAMAKA_AMOUNT:patch.gamaka_amount[i]=bound(v,0,1000);break;
 case MD_VADI:patch.vadi=bound(v,-1,11);break;
 case MD_MOTION_EVERY:patch.motion_every=bound(v,0,4);break;
 case MD_MORPH_TO:morph_to(bound(v,0,MD_BIOMES-1));break;
 case MD_MORPH:if(morph_on){morph_pos=bound(v,0,1000);morph_f=(float)morph_pos;morph_apply();}break;
 case MD_MORPH_BARS:morph_bars=bound(v,0,5);break;
 case MD_MORPH_SCOPE:morph_scope=bound(v,0,MD_MORPH_SCOPES-1);if(morph_on)morph_apply();break;
 case MD_MORPH_KEEP:morph_keep();break;
 case MD_DRIFT_SLOW:drift_arm(bound(v,0,6));break;
 case MD_DRIFT_AMOUNT:drift_amount=bound(v,0,2);break;
 case MD_DRIFT_VOYAGE:drift_voyage=bound(v,0,2);break;
 case MD_DRIFT_SCOPE:drift_scope=bound(v,0,MD_MORPH_SCOPES-1);break;
 case MD_DRIFT_HOME:drift_new_home();break;
 case MD_CLEAR_PHRASE:for(int k=0;k<16;k++)patch.ext[i].notes[k]=-1;patch.ext[i].edit_step=0;break;
 case MD_SAMVADI:patch.samvadi=bound(v,-1,11);break;
 case MD_PANIC:if(playing&&routing.clock&&!clock_source)out_push(0xfc,0,0);playing=0;out_all_off();out_panic();clear_audio();for(unsigned j=0;j<6;j++)reset_voice(j);break;
 }
}
// While a morph runs, what a command changes is pinned into A and B (and home, for a knob during DERIVE).
static int returnable; // ENV has something to go back to; recomputed after commands and each step, read by the UI
static void apply(Command e) {
 const bool pin=morph_on&&e.c!=MD_RETURN&&e.c!=MD_SCENE&&e.c!=MD_PLAY&&e.c!=MD_TRIGGER&&e.c!=MD_PANIC&&e.c!=MD_QUALITY&&e.c<MD_MORPH_TO;
 if(pin)morph_before=patch;
 apply_command(e);
 if(pin&&morph_on)morph_pin(e.c!=MD_DICE&&e.c!=MD_MUTATE); // REC and DICE last the glide; ENV still goes home
}
static void tick() {
 float duration=MD_RATE*60.0f/(patch.tempo*4.0f)*(1+(step%2?-1:1)*unit(patch.swing)*.5f);
 if((step%32)==0) {if(patch.locked)seqrng=patch.seed;else patch.seed=random_word(seqrng)|1;}
 static const int degree[7]={0,2,3,5,7,10,12};
 float tension=.6f;unsigned dropped=0;orbit_step_len=duration;orbit_since=0; // Rhizome: the orbits measure the step from here
 if(patch.form){unsigned bars=form_bar_len[bound(patch.form_bars,0,3)],bar=step/16,sec=(bar/bars)%4,cyc=bar/(bars*4);
  tension=form_tension_of[sec];dropped=form_mask(cyc,sec);
  stats.form_section=sec;stats.form_cycle=cyc;stats.form_drop_mask=dropped;stats.form_pos=(float)(step%(16*bars))/(16*bars);}
 orbit_rest=dropped;
 for(unsigned i=0;i<6;i++) {
  if(patch.orbit[i].gates){stats.step_played[i]=stats.step_skipped[i]=0;continue;} // Rhizome: an orbit plays this track
  const md_track &t=patch.track[i];if(step%(unsigned)t.speed)continue;
  unsigned local=step/t.speed;
  int hits=bound(t.hits+(patch.macro[0]-500)*t.length/1500+(int)floorf(lfo_mod[i][MD_TO_HITS]*t.length+.5f)+(patch.form?(int)floorf((tension-.6f)*unit(patch.form)*t.length*.8f+.5f):0),0,t.length);
  unsigned roll=random_word(seqrng);
  // LFO on ROTATE: the pattern turns by that many steps (either way), on top of the knob.
  const int rot=((t.rotate+(int)floorf(lfo_mod[i][MD_TO_ROTATE]*t.length+.5f))%t.length+t.length)%t.length;
  stats.hits_now[i]=hits;stats.rotate_now[i]=rot;
  const bool hit=patch.ext[i].bass_mode==2||md_hit(local,hits,t.length,rot),rest=(dropped>>i)&1;
  bool pass=roll%1000<(unsigned)t.prob;int gen_off=0;const bool gen=patch.gen[i].source&&hit&&!t.mute&&!rest;
  if(gen)gen_off=gen_step(i,pass);
  {const uint32_t bit=1u<<(local%(unsigned)t.length);
   stats.step_played[i]&=~bit;stats.step_skipped[i]&=~bit;if(hit&&!t.mute&&!rest){if(pass)stats.step_played[i]|=bit;else stats.step_skipped[i]|=bit;}}
  if(hit&&pass&&!t.mute&&!rest) {
   int note=t.note;float pitch;
   const md_extension &e=patch.ext[i];
   if(e.bass_mode){int off=e.notes[local%(e.bass_mode==2?(t.length>16?16:t.length):16)];if(off<0)continue;note+=off;
    // Marbles-inspired remembered variation: deterministic for each seed/position,
    // independent of audio noise and stable across repeated phrases when locked.
    unsigned mr=patch.seed^(i*0x9e3779b9u)^((local%16+1)*0x85ebca6bu);unsigned choice=random_word(mr);
    if((local%4)!=0&&choice%1000<(unsigned)e.marbles){static const int interval[]={0,3,5,7};note=t.note+interval[(choice>>12)&3];}
   }
   if(t.model>=MD_FM) {
    if(gen&&!e.bass_mode)note+=gen_off;
    else if(!e.bass_mode&&patch.motion_every){const unsigned every=1u<<bound(patch.motion_every,0,4); // a new variation on the beat, held in between
     if(local%every==0)motion_held[i]=(roll>>10)%1000<(unsigned)patch.macro[1]?degree[(roll>>20)%7]:0;note+=motion_held[i];}
    else if(!e.bass_mode&&(roll>>10)%1000<(unsigned)patch.macro[1])note+=degree[(roll>>20)%7];
    {int target=note+patch.transpose+(int)floorf(lfo_mod[i][MD_TO_NOTES]*12+.5f); /* LFO steps stay in the scale */
     // Pivot notes: a generated note within two semitones of the vadi (or samvadi) is drawn to it, a third of the time.
     if(patch.vadi>=0||patch.samvadi>=0){const int pivots[2]={patch.vadi,patch.samvadi};unsigned pr=roll^0x5bd1e995u;random_word(pr);
      if(pr%3==0)for(int k=0;k<2;k++){if(pivots[k]<0)continue;int pc=(patch.root+pivots[k])%12,d=((pc-target)%12+18)%12-6;if(d>=-2&&d<=2&&d){target+=d;break;}}}pitch=quantize_any((float)target,patch.root,patch.scale,target<last_generated[i]);note=(int)floorf(pitch+.5f);last_generated[i]=note;}
   }
   float velocity=.8f+unit(t.accent)*((local%t.length)==0?.2f:-.25f);
   if(t.model>=MD_FM&&(patch.vadi>=0||patch.samvadi>=0)){int pc=((note-patch.root)%12+12)%12;if(pc==patch.vadi)velocity=fminf(1.0f,velocity+.15f);else if(pc==patch.samvadi)velocity=fminf(1.0f,velocity+.08f);} // pivots sing out
   if(t.model<MD_FM)pitch=(float)note;
   fire(i,pitch,velocity);out_note(i,pitch,velocity,duration*.5f/t.ratchet);
   repeats[i]={(duration/t.ratchet),duration/t.ratchet,pitch,velocity,(unsigned)t.ratchet-1};
  }
 }
 beat_wraps=step/384;beat=(float)(step%384);beat_limit=beat+1;
 morph_step();drift_step(duration);returnable=memcmp(&previous,&patch,sizeof patch)!=0;
 stats.step=step++;untilstep+=duration;
}
// Profiling (stats.prof): ticks spent in each part, turned into percent of a second of audio once a second.
static uint32_t prof_acc[6],prof_frames;
static void quantum(float *lr,unsigned n) {
 const uint32_t pt0=plat_ticks();
 // Conservative admission guard from 0.4.1 FM-1 costs with full-rate tape, LP24 and maximum repeats.
 // Count unmuted slots before playback so the tape cannot prefill four FULL grains
 // immediately before a dense six-voice attack. Existing audible tails also count.
 if(patch.quality){static const unsigned weight[MD_MODELS]={8,13,8,18,10,26,17,8,8,30,10,12,7,13};unsigned cost=7;
  // Monades' weights for its voices; Rhizome's measured on the FM-1 on 4 November 2026 (SIX-OP, STRING MACHINE, the
  // base, the tape and HALL together, Monades' grains).
  if(fx_active||patch.fx_level)cost+=14;if(patch.macro[3])cost+=8;
  if(patch.chorus)cost+=3;if(patch.send_hp||patch.room_diffusion)cost+=2; // Monades estimates, pending FM-1 measurement
  for(unsigned j=0;j<6;j++)if(!patch.track[j].mute&&patch.ext[j].warps)cost+=2;
  for(unsigned i=0;i<6;i++)if(voices[i].active||!patch.track[i].mute)cost+=weight[patch.track[i].model];
  if(cost>85){patch.quality=0;stats.quality_reductions++;for(unsigned i=0;i<6;i++){voices[i].eco_phase=0;voices[i].eco_previous=voices[i].eco_next=0;voices[i].fade=0;}}
 }

 // A shared mode budget also applies in ECO. All six tracks remain audible.
 unsigned element_count=0;
 for(unsigned j=0;j<6;j++)if(patch.track[j].model==MD_ELEMENTS&&(voices[j].active||!patch.track[j].mute))element_count++;
 unsigned element_modes=element_count>1?16/element_count:(patch.quality?16:8);
 element_modes=element_modes<2?2:element_modes;stats.element_modes=element_modes;
 const float block_ratio=n>32?n/32.0f:1.0f;
 const float slew25=n>32?.025f*block_ratio*(1-.0125f*(block_ratio-1)):.025f;
 const float slew40=n>32?.04f*block_ratio*(1-.02f*(block_ratio-1)):.04f;
 const float slew35=n>32?.035f*block_ratio*(1-.0175f*(block_ratio-1)):.035f;
 for(unsigned i=0;i<6;i++)stats.track_peak[i]*=n>32?.984064f:.992f;
 {const float meter_decay=n>32?.984064f:.992f;for(unsigned j=0;j<3;j++)stats.fx_peak[j]*=meter_decay;stats.out_peak[0]*=meter_decay;stats.out_peak[1]*=meter_decay;stats.limit_peak*=meter_decay;}
 for(unsigned j=0;j<4;j++)smooth[j]+=(unit(patch.macro[j])-smooth[j])*slew25;
 lfo_update(n);
 float delay_send[Q]={},room_send[Q]={};
 float send[Q]={},out[Q],aux[Q],tmp[Q],exc[Q],duck[Q];memset(lr,0,n*2*sizeof(float));
 float duckdec=1.0f-1.0f/(MD_RATE*(.04f+unit(patch.duck_release)*1.46f));
 for(unsigned k=0;k<n;k++){duck[k]=1-duckenv*unit(patch.duck_amount)*.9f;duckenv*=duckdec;}
 stats.duck_gain=n?duck[n-1]:1;
 const uint32_t pt1=plat_ticks();prof_acc[0]+=pt1-pt0; // the sequencer, the orbits, the LFOs
 for(unsigned i=0;i<6;i++) {
  md_track &t=patch.track[i];Voice &v=voices[i];bool trig=v.trigger;v.trigger=0;
  if(trig) {v.active=1;v.attacking=t.attack>0; if(!v.attacking)v.env=1;v.excite=1;v.lpg=1;}
  /* Only FM/VA have an observable final envelope. Never cut resonator/drum tails. */
  if(!v.active){stats.skipped_quanta++;continue;}stats.voice_quanta++;
  const md_extension &e=patch.ext[i];const float *lm=lfo_mod[i];
  // LFO targets; with depth 0 every value below is exactly the knob's.
  const float harm=lm[MD_TO_HARMONICS]?clamp01(unit(t.harm)+lm[MD_TO_HARMONICS]*.5f):unit(t.harm),shp=lm[MD_TO_SHAPE]?clamp01(unit(t.shape)+lm[MD_TO_SHAPE]*.5f):unit(t.shape);
  const float decay_u=lm[MD_TO_DECAY]?clamp01(unit(t.decay)+lm[MD_TO_DECAY]*.5f):unit(t.decay),att=lm[MD_TO_ATTACK]?clamp01(unit(t.attack)+lm[MD_TO_ATTACK]*.5f):unit(t.attack);
  v.note+=(v.target_note-v.note)*(e.glide?n/(float)(n+MD_RATE*.35f*unit(e.glide)):1.0f);
  v.tide_phase+=n/(MD_RATE*(.5f+unit(e.tides_rate)*19.5f));if(v.tide_phase>=1)v.tide_phase-=1;
  stats.tide_phase[i]=v.tide_phase;
  float tidal=(1.0f-4.0f*fabsf(v.tide_phase-.5f))*unit(e.tides);
  float tone=fminf(.98f,fmaxf(t.model==MD_HARMONIC?0.0f:.02f,unit(t.tone)+(smooth[2]-.5f)*.65f+lm[MD_TO_TONE]*.5f));
  bend_smooth[i]+=(bend[i]-bend_smooth[i])*slew40;
  float ornament=0;if(v.orn_style){ornament=gamaka_shape(v.orn_style,patch.gamaka_amount[i],v.orn_from,v.orn_age*1000);v.orn_age+=n*(1.0f/MD_RATE);}
  stats.gamaka_now[i]=ornament;stats.gamaka_age[i]=v.orn_age*1000;
  float played_note=v.note+bend_smooth[i]+lm[MD_TO_PITCH]*12.0f+ornament;
  float f=440.0f*stmlib::SemitonesToRatio(played_note-69.0f)/MD_RATE;
  // Rhizome: SIX-OP and STRING MACHINE get Plaits' gate, high from the note for as long as its envelope still sounds
  // (SIX-OP's DX7 envelopes hold on it), and an accent below 1 (SIX-OP reads one table entry past it at 1).
  // SIX-OP has a single DX7 voice: its envelopes start again only after a block with the gate low, so the gate drops
  // for the note's first block (1.5 ms) and rises on the next; with the gate held, every note after the first was silent.
  const bool held=v.attacking||v.env>.3f||v.midi_gate>0;
  const bool gate=t.model==MD_SIXOP?!trig&&held:t.model==MD_STRINGS&&(trig||held);
  plaits::EngineParameters p={(int)((trig?plaits::TRIGGER_RISING_EDGE:0)|(gate?plaits::TRIGGER_HIGH:0)),played_note,tone,shp,harm*.999f,t.model>=MD_SIXOP?fminf(v.velocity,.999f):v.velocity};bool done=false;
  memset(out,0,sizeof out);memset(aux,0,sizeof aux);
  // ECO: FM, VA and HARMONIC at half rate as in Monades, and Rhizome's two (SIX-OP's envelopes rebuilt for the half rate).
  bool eco=patch.quality==0&&(t.model==MD_FM||t.model==MD_VA||t.model==MD_HARMONIC||t.model==MD_SIXOP||t.model==MD_STRINGS);
  if(t.model==MD_SIXOP&&v.engine&&v.half!=(int)eco){static_cast<plaits::SixOpEngine*>(v.engine)->set_half_rate(eco);v.half=eco;}
  unsigned engine_n=eco?(n+(v.eco_phase?0:1))/2:n;
  if(eco)p.note+=12.0f; // same concert pitch at 22.05 kHz; linear reconstruction below.
  const uint32_t e0=plat_ticks();
  switch(t.model) {
   case MD_SIXOP:case MD_STRINGS:if(engine_n&&v.engine)v.engine->Render(p,out,aux,engine_n,&done);break;
   case MD_KICK:v.kick.Render(false,trig,v.velocity,f,tone,decay_u,harm,.2f,out,n);break;
   case MD_SNARE:v.snare.Render(false,trig,v.velocity,f,tone,decay_u,harm,out,n);break;
   case MD_HAT:v.hat.Render(false,trig,v.velocity,f,tone,decay_u,harm,tmp,exc,out,n);break;
   case MD_FM:if(engine_n)v.fm.Render(p,out,nullptr,engine_n,&done);break;
   case MD_VA:if(engine_n)v.va.Render(p,out,nullptr,engine_n,&done);break;
   case MD_HARMONIC:if(engine_n)v.additive.Render(p,out,nullptr,engine_n,&done);break;
   case MD_ELEMENTS:{
    elements::Exciter &strike=element_strike[i];elements::Resonator &body=element_body[i];
    if(trig)v.strike_gate=MD_RATE*(.03f+.5f*shp);
    unsigned strike_flags=(trig?elements::EXCITER_FLAG_RISING_EDGE:0)|(v.strike_gate>0?elements::EXCITER_FLAG_GATE:0);v.strike_gate=fmaxf(0,v.strike_gate-n);
    strike.set_model(t.shape<334?elements::EXCITER_MODEL_MALLET:t.shape<667?elements::EXCITER_MODEL_PLECTRUM:elements::EXCITER_MODEL_PARTICLES);
    // In the Plectrum region, keep its main pulse 1.7-5.2 ms after the primer.
    // The original 14-38 ms mapping was continually restarted by 220 BPM
    // four-ratchet rolls (17 ms spacing), leaving only the tiny primer audible.
    strike.set_parameter(t.shape>=334&&t.shape<667?.05f+.15f*(t.shape-334)/332.0f:.1f+.8f*shp);
    strike.set_timbre(fminf(.95f,tone));strike.Process(strike_flags,exc,n);
    body.set_frequency(f);body.set_geometry(fminf(.999f,harm));body.set_brightness(tone);body.set_damping(decay_u*.8f);body.set_position(.3f);body.set_resolution(element_modes);
    memset(tmp,0,sizeof tmp);body.Process(tmp,exc,out,aux,n);break;}
   case MD_SATIN:{
    // Original rounded wavetable oscillator. Reduce harmonics continuously
    // before Nyquist; interpolate phase and waveform, with no oversampling RAM.
    v.satin_morph+=(harm*2.999f-v.satin_morph)*slew35;
    v.satin_tone+=(tone-v.satin_tone)*slew35;
    int shape=(int)v.satin_morph;float morph=v.satin_morph-shape;
    v.mellow_drift+=n/(MD_RATE*(9.0f+i));if(v.mellow_drift>=1)v.mellow_drift-=1;
    float f2=f*stmlib::SemitonesToRatio(.13f*shp*plaits::Sine(v.mellow_drift));
    int band=3;float highest=fmaxf(f,f2);
    while(band>0&&highest*(1<<band)>.45f)band--;
    float fade=band?fminf(1.0f,fmaxf(0.0f,(.45f-highest*(1<<band))/.1125f)):1;
    for(unsigned k=0;k<n;k++){
     v.sub_phase+=f;if(v.sub_phase>=1)v.sub_phase-=1;
     v.mellow_phase+=f2;if(v.mellow_phase>=1)v.mellow_phase-=1;
     float sum=0;float phases[2]={v.sub_phase,v.mellow_phase};
     for(int osc=0;osc<2;osc++){
      float index=phases[osc]*256;int at=(int)index;float frac=index-at;
      const float *a=satin_table[band][shape],*b=satin_table[band][shape+1];
      float x=a[at]+(a[at+1]-a[at])*frac,y=b[at]+(b[at+1]-b[at])*frac;
      float wave=x+(y-x)*morph;
      if(band&&fade<1){a=satin_table[band-1][shape];b=satin_table[band-1][shape+1];x=a[at]+(a[at+1]-a[at])*frac;y=b[at]+(b[at+1]-b[at])*frac;float low=x+(y-x)*morph;wave=low+(wave-low)*fade;}
      sum+=wave;
     }
     float fundamental=plaits::Sine(v.sub_phase);
     out[k]=.76f*fundamental+(sum*.42f-.76f*fundamental)*v.satin_tone;
    }break;}
   case MD_MELLOW:{
    // Finite odd-harmonic triangle blend: no discontinuous saw/square aliasing.
    // Small, slow independent drift; Tone adds rounded harmonics, Shape detune.
    v.mellow_drift+=n/(MD_RATE*(8.0f+i));if(v.mellow_drift>=1)v.mellow_drift-=1;
    float detune=stmlib::SemitonesToRatio(.10f*shp*plaits::Sine(v.mellow_drift));
    for(unsigned k=0;k<n;k++){
     v.sub_phase+=f;if(v.sub_phase>=1)v.sub_phase-=1;
     v.mellow_phase+=f*detune;if(v.mellow_phase>=1)v.mellow_phase-=1;
     float a=plaits::Sine(v.sub_phase),b=plaits::Sine(v.mellow_phase);
     float tri=(f*5<.45f?plaits::Sine(v.sub_phase*5)*.04f:0)-(f*3<.45f?plaits::Sine(v.sub_phase*3)/9:0);
     out[k]=.38f*(a+b)+tone*.25f*tri+.08f*harm*plaits::Sine(v.sub_phase*2);
    }break;}
   case MD_BASS:
    for(unsigned k=0;k<n;k++){v.sub_phase+=f*.5f;if(v.sub_phase>=1)v.sub_phase-=1;
     out[k]=.65f*plaits::Sine(v.sub_phase*2)+.2f*harm*plaits::Sine(v.sub_phase)+.12f*tone*plaits::Sine(v.sub_phase*4);}
    break;
   case MD_CHOIR:{
    static const int chord[4][3]={{0,3,7},{0,4,7},{0,5,7},{0,7,12}};
    int ci=bound(t.harm*4/1001,0,3);
    for(int j=0;j<3;j++){
     int note=md_quantize((int)played_note+chord[ci][j],patch.root,patch.scale);
     float hz=440.0f*stmlib::SemitonesToRatio(note-69.0f+(j-1)*shp*.20f)/MD_RATE;
     v.choir[j].Render(hz,j?(.25f+.12f*tone):.45f,out,n);
    }break;}
   case MD_RINGS:
    v.rings.set_frequency(f);v.rings.set_structure(harm*.999f);v.rings.set_brightness(tone);
    v.rings.set_damping(decay_u*.9f);v.rings.set_position(.35f);v.rings.set_resolution(patch.quality?24:8);
    for(unsigned k=0;k<n;k++){exc[k]=noise()*v.excite*.65f*v.velocity;v.excite*=.985f;}
    v.rings.Process(exc,out,aux,n);break;
  }
  prof_acc[1]+=plat_ticks()-e0;
  if(eco){memcpy(tmp,out,engine_n*sizeof(float));unsigned j=0;
   for(unsigned k=0;k<n;k++){if(!v.eco_phase){v.eco_previous=v.eco_next;v.eco_next=tmp[j++];out[k]=(v.eco_previous+v.eco_next)*.5f;}else out[k]=v.eco_next;v.eco_phase^=1;}
  }
  v.breath_phase+=n/(MD_RATE*(7.0f+i*2.3f));if(v.breath_phase>=1)v.breath_phase-=1;
  float cutoff=unit(t.cutoff)+tidal*.2f+unit(t.breathe)*.13f*plaits::Sine(v.breath_phase)+(smooth[2]-.5f)*.18f+lm[MD_TO_CUTOFF]*.5f;
  if(t.filter==3)cutoff-=.55f*(1-v.lpg);
  cutoff=fminf(1.0f,fmaxf(0.0f,cutoff));
  v.cut_smooth+=(cutoff-v.cut_smooth)*slew40;v.res_smooth+=(clamp01(unit(t.resonance)+lm[MD_TO_RESONANCE]*.5f)-v.res_smooth)*slew40;
  stats.cutoff_now[i]=cutoff_hz(v.cut_smooth);stats.q_now[i]=filter_q(v.res_smooth);
  if(t.filter){float fc=cutoff_hz(v.cut_smooth)/MD_RATE;
   v.filter1.set_f_q<stmlib::FREQUENCY_ACCURATE>(fc,filter_q(v.res_smooth));
   if(t.filter==2||t.filter==7)v.filter2.set_f_q<stmlib::FREQUENCY_ACCURATE>(fc,.707f);}
  float target=t.mute?0:unit(t.level)*.24f*(1+lm[MD_TO_LEVEL]);
  float pan=clamp01(unit(t.pan)+lm[MD_TO_PAN]*.5f),dec=1.0f-1.0f/(MD_RATE*(.06f+decay_u*decay_u*16.0f));
  float attackstep=1.0f/(1+MD_RATE*8.0f*att*att),sendlevel=clamp01(unit(t.send)+lm[MD_TO_GRAIN_SEND]*.5f);
  const bool melodic=md_has_envelope(t.model);
  const float velocity=melodic?v.velocity:1,warp_target=unit(e.warps),ds=clamp01(unit(e.delay_send)+lm[MD_TO_ECHO_SEND]*.5f),rs=clamp01(unit(e.reverb_send)+lm[MD_TO_ROOM_SEND]*.5f+comet_swell),expr=expression[i];
  float track_peak=stats.track_peak[i];
  const float plaits_gain=t.model==MD_STRINGS?2.54f:t.model==MD_SIXOP?1.08f:1.0f; // Rhizome: its two voices at FM's level (measured 3 November 2026)
  // Keep the exact envelope -> filter -> gain order, but let the SVF retain
  // coefficients and state in registers across a tile instead of the mixer.
  for(unsigned k=0;k<n;k++){
   float x=t.model==MD_ELEMENTS?(out[k]+aux[k]*.3f)*3.0f*v.velocity:t.model==MD_RINGS?(out[k]+aux[k])*12.0f:out[k]*plaits_gain;
   if(melodic){if(v.midi_gate<0){v.env*=.9975f;v.attacking=0;}else if(v.attacking){v.env+=attackstep;if(v.env>=1){v.env=1;v.attacking=0;}}else if(v.midi_gate==0)v.env*=dec;x*=v.env;}
   out[k]=x;
  }
  // Rhizome: a voice whose envelope has died away (-70 dB, nothing held) goes to sleep: its engine is no longer computed
  // until its next note (on the FM-1 a sounding voice costs 6 to 13 % of the processor, measured 4 November 2026).
  if(!v.attacking&&v.midi_gate<=0&&v.env<.0003f){v.env=0;v.active=0;}
  if(t.filter==4||t.filter==7){v.filter1.Process<stmlib::FILTER_MODE_HIGH_PASS>(out,out,n);if(t.filter==7)v.filter2.Process<stmlib::FILTER_MODE_HIGH_PASS>(out,out,n);}
  else if(t.filter==5)v.filter1.Process<stmlib::FILTER_MODE_BAND_PASS_NORMALIZED>(out,out,n);
  else if(t.filter==6){v.filter1.Process<stmlib::FILTER_MODE_BAND_PASS_NORMALIZED>(out,tmp,n);for(unsigned k=0;k<n;k++)out[k]-=tmp[k];} // notch = input - band
  else if(t.filter){v.filter1.Process<stmlib::FILTER_MODE_LOW_PASS>(out,out,n);if(t.filter==2)v.filter2.Process<stmlib::FILTER_MODE_LOW_PASS>(out,out,n);}
  if(i==scope_track){unsigned stride=(unsigned)(1.0f/(f*32.0f));stride=stride<1?1:stride>32?32:stride;unsigned ph=scope_phase[i],ps=scope_pos[i];
   for(unsigned k=0;k<n;k++)if(++ph>=stride){ph=0;float y=out[k]*8192.0f;scope_buf[i][ps]=(int16_t)(y>32767?32767:y<-32767?-32767:y);ps=(ps+1)&127;}
   scope_phase[i]=ph;scope_pos[i]=ps;}
  // Local scalars prevent output-buffer aliasing from forcing repeated Voice
  // loads/stores. Same arithmetic and order; commit once after the tile.
  float mix_amp=v.amp;float mix_fade=v.fade;float mix_lpg=v.lpg;float mix_warp_smooth=v.warp_smooth;float mix_warp_phase=v.warp_phase;float mix_expression_smooth=v.expression_smooth;float mix_pan_smooth=v.pan_smooth;float mix_delay_send_smooth=v.delay_send_smooth;float mix_reverb_send_smooth=v.reverb_send_smooth;
  const bool duck_this_voice=(int)i!=patch.duck_source;
  for(unsigned k=0;k<n;k++) {
   mix_amp+=(target-mix_amp)*.004f;mix_fade+=(1-mix_fade)*.004f;
   float x=out[k];if(t.filter==3)x*=mix_lpg;
   mix_lpg*=dec;
   if(!isfinite(x)) {stats.nonfinite++;x=0;}
   // Individual oscillator outputs remain linear; tape and final mix retain peak protection.
   // Warps-inspired ring blending, not a claim to port the complete module.
   mix_warp_smooth+=(warp_target-mix_warp_smooth)*.002f;
   if(mix_warp_smooth>.000001f){mix_warp_phase+=f;if(mix_warp_phase>=1)mix_warp_phase-=1;x*=1-mix_warp_smooth*.5f+mix_warp_smooth*.5f*plaits::Sine(mix_warp_phase);}
   mix_expression_smooth+=(expr-mix_expression_smooth)*.003f;mix_pan_smooth+=(pan-mix_pan_smooth)*.003f;
   x=x*mix_amp*mix_fade*mix_expression_smooth*velocity;if(duck_this_voice)x*=duck[k];
   track_peak=fmaxf(track_peak,fabsf(x));
   lr[2*k]+=x*(1-mix_pan_smooth);lr[2*k+1]+=x*mix_pan_smooth;send[k]+=x*sendlevel;
   mix_delay_send_smooth+=(ds-mix_delay_send_smooth)*.002f;mix_reverb_send_smooth+=(rs-mix_reverb_send_smooth)*.002f;
   delay_send[k]+=x*mix_delay_send_smooth;room_send[k]+=x*mix_reverb_send_smooth;
  }
  v.amp=mix_amp;v.fade=mix_fade;v.lpg=mix_lpg;v.warp_smooth=mix_warp_smooth;v.warp_phase=mix_warp_phase;v.expression_smooth=mix_expression_smooth;v.pan_smooth=mix_pan_smooth;v.delay_send_smooth=mix_delay_send_smooth;v.reverb_send_smooth=mix_reverb_send_smooth;
  stats.track_peak[i]=track_peak;
  stats.env_now[i]=melodic?v.env:0;stats.env_attacking[i]=melodic&&v.attacking;
  if(melodic&&!v.attacking&&v.env<.000001f)v.active=0;
 }
 // Rhizome: the comet's FILTER, a 12 dB low-pass on the mix: from 18 kHz (wide open) down to 18 kHz / 2^(AMOUNT x 0.06)
 // (280 Hz at 100) while the comet is far, opening as it nears the star. Off: not computed, the mix untouched.
 {const bool on=patch.orbit_comet&&patch.comet_event==MD_COMET_FILTER&&patch.comet_amount>0;
  if(on&&!comet_filtering){comet_lp[0].Init();comet_lp[1].Init();}comet_filtering=on;stats.comet_filter_hz=0;
  if(on){const float span=patch.comet_amount*.06f,hz=18000/exp2f(span*(1-comet_open));
   for(int c=0;c<2;c++)comet_lp[c].set_f_q<stmlib::FREQUENCY_ACCURATE>(hz/MD_RATE,.9f);stats.comet_filter_hz=hz;}}
 const float echo_feedback=lfo_echo_fb?fminf(.8f,clamp01(unit(patch.delay_fb)+lfo_echo_fb*.5f)):unit(patch.delay_fb),effect_return=unit(patch.fx_level)+(1.4f-unit(patch.fx_level))*comet_swell;
 reel_block(n,echo_ms_now()*(1+lfo_echo_time*.5f),lfo_echo_time?.002f:.00005f); // Rhizome: the tape's speed, wow, dropouts
 const float echo_tone=tone_coef(patch.echo_tone,.04f,.25f,.9f);
 {const uint32_t pt2=plat_ticks();prof_acc[2]+=pt2-pt1;} // the voices (engines included: [2] minus [1] is the chain around them)
 hall_block(n); // Rhizome: the HALL's settings for this block
 const bool send_hp=patch.send_hp>0;if(send_hp&&patch.send_hp!=send_hp_set){send_hp_set=patch.send_hp;float fc=md_send_hp_hz(patch.send_hp)/(float)MD_RATE;send_hp_filter[0].set_f_q<stmlib::FREQUENCY_ACCURATE>(fc,.707f);send_hp_filter[1].set_f_q<stmlib::FREQUENCY_ACCURATE>(fc,.707f);}
 if(patch.chorus&&!fx_shed){ // de-interleave in place through the scratch buffers: no extra memory
  for(unsigned k=0;k<n;k++){out[k]=lr[2*k];aux[k]=lr[2*k+1];}
  md_chorus_process(out,aux,n,patch.chorus*.001f,patch.chorus_depth*.001f);
  for(unsigned k=0;k<n;k++){lr[2*k]=out[k];lr[2*k+1]=aux[k];}
 }
 const bool frozen=patch.freeze||orbit_comet_frozen(); // Rhizome: the comet can freeze the tape
 static float bead_l[Q],bead_r[Q];const uint32_t pg=plat_ticks();beads_block(send,n,bead_l,bead_r,frozen);const uint32_t pg2=plat_ticks();prof_acc[3]+=pg2-pg; // Rhizome: Beads, on the grain sends (static: off the audio stack)
 float fx_peak[3]={stats.fx_peak[0],stats.fx_peak[1],stats.fx_peak[2]},out_peak[2]={stats.out_peak[0],stats.out_peak[1]},limit_peak=stats.limit_peak;
 for(unsigned k=0;k<n;k++) {
  const float wet[2]={bead_l[k],bead_r[k]};
  float effect[2]={};
  if(patch.fx_level||comet_swell>.001f)fx_active=true;
  if(fx_active){
  float echo_in=delay_send[k],room_input=room_send[k];
  if(send_hp){echo_in=send_hp_filter[0].Process<stmlib::FILTER_MODE_HIGH_PASS>(echo_in);room_input=send_hp_filter[1].Process<stmlib::FILTER_MODE_HIGH_PASS>(room_input);}
  float tape_l,tape_r;reel_sample(echo_in,echo_feedback,echo_tone,&tape_l,&tape_r); // Rhizome: the tape echo
  float hp=room_input-fx_highpass_in+.985f*fx_highpass_out;fx_highpass_in=room_input;fx_highpass_out=hp;
  float hall_l,hall_r;hall_sample(hp,&hall_l,&hall_r); // Rhizome: the HALL
  fx_smooth+=(effect_return-fx_smooth)*.002f;
  effect[0]=fx_smooth*(tape_l+hall_l*HALL_OUT);effect[1]=fx_smooth*(tape_r+hall_r*HALL_OUT);
  // Meters only: the returns as they reach the final 1.9x mix stage.
  fx_peak[1]=fmaxf(fx_peak[1],fabsf(fx_smooth*(tape_l+tape_r))*.95f);fx_peak[2]=fmaxf(fx_peak[2],fx_smooth*fmaxf(fabsf(hall_l),fabsf(hall_r))*HALL_OUT*.95f);
  }
  fx_peak[0]=fmaxf(fx_peak[0],fmaxf(fabsf(wet[0]),fabsf(wet[1]))*smooth[3]*3.23f*duck[k]);
  for(unsigned c=0;c<2;c++) {
   float x=effect[c]+lr[2*k+c]+wet[c]*smooth[3]*1.7f*duck[k];
   if(comet_filtering)x=comet_lp[c].Process<stmlib::FILTER_MODE_LOW_PASS>(x); // the comet's FILTER, on the whole mix
   float y=x-dcx[c]+.9997f*dcy[c];dcx[c]=x;dcy[c]=y;
   limit_peak=fmaxf(limit_peak,fabsf(y*1.9f));
   x=clip(y*1.9f);lr[2*k+c]=x;
   if(fabsf(x)>stats.peak)stats.peak=fabsf(x);
   out_peak[c]=fmaxf(out_peak[c],fabsf(x));
  }
 }
 for(unsigned j=0;j<3;j++)stats.fx_peak[j]=fx_peak[j];
 stats.out_peak[0]=out_peak[0];stats.out_peak[1]=out_peak[1];stats.limit_peak=limit_peak;
 stats.frames+=n;
 {const uint32_t pe=plat_ticks();prof_acc[4]+=pe-pg2;prof_acc[5]+=pe-pt0;prof_frames+=n;
  if(prof_frames>=MD_RATE){for(int k=0;k<6;k++){stats.prof[k]=prof_acc[k]*(100.0f/(PLAT_TICKS*1e6f));prof_acc[k]=0;}stats.prof[2]-=stats.prof[1];prof_frames=0;}}
}
}
extern "C" {
int md_hit(unsigned s,unsigned h,unsigned l,unsigned r) {if(!l||l>32)return 0;h=h>l?l:h;return (((s+r)%l)*h)%l<h;}
int md_orbit_steps(int period){return (int)(orbit_steps_at(period,patch.tempo)+.5f);}
int md_orbit_tenths(int period){period=bound(period,0,MD_ORBIT_PERIODS-1);return period<MD_ORBIT_TEMPO_PERIODS?0:orbit_free_tenths[period-MD_ORBIT_TEMPO_PERIODS];}
const char *md_orbit_system_name(int s,int brief){const OrbitSystem &y=orbit_systems[bound(s,0,MD_ORBIT_SYSTEMS-1)];return brief?y.brief:y.name;}
const char *md_orbit_system_about(int s){return orbit_systems[bound(s,0,MD_ORBIT_SYSTEMS-1)].about;}
int md_orbit_comet_steps(int comet){return (int)orbit_comet_steps(bound(comet,0,MD_COMETS-1));} // WANDER: its turn now
int md_orbit_moon_steps(int spin){return orbit_moon_steps_of[bound(spin,0,MD_MOON_SPINS-1)];}
// The gates of a track: its turn's steps, the euclidean hits among them, each as a direction (top = -pi/2, clockwise).
// DENSITY adds or takes gates as it adds or takes hits on a rhythm (up to a third of 16 each way); a body keeps its orbit.
int md_orbit_gate_angles(const md_patch *p,int track,float *phi){
 const md_orbit &o=p->orbit[bound(track,0,5)];const int n=md_orbit_tenths(o.period)?16:md_orbit_steps(o.period),most=n<MD_ORBIT_GATES_MAX?n:MD_ORBIT_GATES_MAX;
 const int k=o.gates?bound(o.gates+(p->macro[0]-500)*(most<16?most:16)/1500,0,most):0;int count=0;
 for(int s=0;s<n&&count<MD_ORBIT_GATES_MAX;s++)if((s*k)%n<k)phi[count++]=-1.57079633f+o.phase*.261799388f+ORBIT_TAU*s/n;
 return count;
}
float md_orbit_eccentric(float mean,float e){return orbit_E(mean,e);}
float md_atan2(float y,float x){return orbit_atan2(y,x);}
void md_init() {
 beads_init(); // Rhizome: the granular bus
 {static const uint8_t *const banks[3]={plaits::syx_bank_0,plaits::syx_bank_1,plaits::syx_bank_2}; // SIX-OP's 96 DX7 sounds
  for(int b=0;b<3;b++)for(int k=0;k<32;k++)sixop_bank[b*32+k].Unpack(banks[b]+k*plaits::fm::Patch::SYX_SIZE);}
 voices=reinterpret_cast<Voice*>(voices_raw);memset(voices_raw,0,sizeof voices_raw);
 memset(&stats,0,sizeof stats);memset(smooth,0,sizeof smooth);memset(dcx,0,sizeof dcx);memset(dcy,0,sizeof dcy);
 out_r=out_w=owed_n=0;memset(&routing,0,sizeof routing);clock_left=0;memset(chan_bend,0,sizeof chan_bend);for(int i=0;i<6;i++)in_chan[i]=out_chan[i]=i+1;
 for(int i=0;i<6;i++)last_generated[i]=60;
 memset(midi_bank,0,sizeof midi_bank);qread=qwrite=0;rng=0x88888;stmlib::Random::Seed(rng);comet_swell=0;comet_open=1;comet_filtering=false;scene(0);orbit_reset();playing=0;clock_source=midi_quantize=clock_pulse=0;clock_age=clock_last=0;clock_interval=0;
}
void md_set_midi_routing(const md_midi_routing *r){
 // Turning output off (or changing the routing) first releases every note still owed on the old routing.
 const int was_clock=routing.clock;
 out_all_off();for(unsigned ch=0;ch<16;ch++)out_bend(ch,0);routing=*r;routing.mode=bound(routing.mode,0,2);routing.clock=!!routing.clock;
 if(playing&&!clock_source&&routing.clock!=was_clock){out_push(routing.clock?0xfa:0xfc,0,0);clock_left=0;} // clock out switched while playing: Start or Stop with it
 for(unsigned z=0;z<8;z++){routing.zone[z].lo=bound(routing.zone[z].lo,0,127);routing.zone[z].hi=bound(routing.zone[z].hi,0,127);routing.zone[z].ch=bound(routing.zone[z].ch,0,16);}
}
void md_set_midi_channels(const int32_t in[6],const int32_t out[6]){
 // A track that stops listening on a channel lets go of the note held there: its note-off would never come.
 // Notes owed on the old out channels are released first, as when the routing changes.
 bool moved=false;for(int i=0;i<6;i++)moved|=bound(out[i],0,16)!=out_chan[i];
 if(moved){out_all_off();for(unsigned ch=0;ch<16;ch++)out_bend(ch,0);}
 for(int i=0;i<6;i++){const int c=bound(in[i],0,16);
  if(c!=in_chan[i]){held[i]=-1;sustain[i]=0;bend[i]=0;if(voices[i].midi_gate>0)voices[i].midi_gate=-1;}
  in_chan[i]=c;out_chan[i]=bound(out[i],0,16);}
}
int md_midi_take(uint8_t m[3]){unsigned r=out_r;if(r==out_w)return 0;fence();const OutMsg &o=outq[r&255];m[0]=o.st;m[1]=o.d1;m[2]=o.d2;fence();out_r=r+1;return 1;}
unsigned md_scope(int track,float *dst){
 // Oldest first; the caller aligns on a rising zero crossing.
 track=bound(track,0,5);scope_track=(unsigned)track;unsigned ps=scope_pos[track];
 for(unsigned k=0;k<128;k++)dst[k]=scope_buf[track][(ps+k)&127]*(1.0f/8192);
 return 128;
}
float md_beads_rate(int density){ // as Beads' trigger generator (kMaxGrainFrequency at its 48 kHz, run at 44.1)
 const float d=density*.001f,top=130.81278f*MD_RATE/48000.0f; // kMaxGrainFrequency (trigger_generator.cc)
 return d<.48f?top*stmlib::SemitonesToRatio(-d*192):d>.52f?.5f*top*stmlib::SemitonesToRatio(-(1-d)*192):0;
}
float md_beads_envelope(float shape,float t){ // as Beads' GrainEnvelope::Start and Render, without the minimum lengths
 const float sustain=fmaxf(1-3*shape,0.0f);float a=(shape-.33333333f)*1.5f;a=a<0?0:a<.5f?a*a*a*4:a;
 const float release=(1-a)*(1-sustain),smooth=a<.5f?2*a:2-2*a;
 float x;if(t<a)x=t/a;else if(t<a+sustain)return 1;else x=release>0?1-(t-a-sustain)/release:0;
 x=x<0?0:x>1?1:x;const float base=t<a?x:(x*x)*(x*x),ss=x*x*(3-2*x);return base+(ss-base)*smooth;
}
float md_beads_seconds(void){return (float)GR_N*gr_step()/MD_RATE;}
void md_tape_view(md_tape *v){
 // Rhizome: the grain tape: the level written along it, the write head, where the next grains start (POSITION), the
 // live grains (how far behind the head they read, how long they have left).
 memcpy(v->peak,tape_peak,sizeof v->peak);v->head=gr_head;v->valid=gr_valid;v->length=GR_N;
 v->freeze=patch.freeze||orbit_comet_frozen();v->zone=unit(patch.grain_pos)*(float)GR_N;v->zone_len=(float)GR_N*.04f;
 int n=0;for(int j=0;j<GR_MAX&&n<MD_TAPE_GRAINS;j++){const GrainVoice &g=gr[j];if(g.age>=g.life)continue;float d=(float)gr_head-g.pos;while(d<0)d+=GR_N;
  v->grain[n].active=1;v->grain[n].behind=d;v->grain[n].len=(float)(g.life-g.age);v->grain[n].env=md_beads_envelope(patch.beads_shape*.001f,g.age*g.inc);n++;}
 for(int j=n;j<MD_TAPE_GRAINS;j++)v->grain[j].active=0;
}
int md_clock_source(void){return clock_source;}
int md_midi_quantize(void){return midi_quantize;}
static void midi_track(unsigned i,unsigned type,unsigned a,unsigned b);
void md_midi(unsigned st,unsigned a,unsigned b,int port){
 a&=127;b&=127;
 if(st>=0xf8){
  if(!clock_source||port!=(clock_source==1?0:2))return;
  if(st==0xfa){step=0;seqrng=patch.seed;clock_pulse=0;playing=1;beat=0;beat_limit=1;beat_wraps=0;gen_reset();clock_age=0;clock_last=stats.frames;clock_interval=0;memset(repeats,0,sizeof repeats);}
  else if(st==0xfb){playing=1;clock_age=0;}
  else if(st==0xfc){playing=0;memset(repeats,0,sizeof repeats);out_all_off();}
  else if(st==0xf8&&playing){unsigned delta=stats.frames-clock_last;clock_last=stats.frames;
   if(delta>=MD_RATE*60/(220*24)&&delta<=MD_RATE*60/(30*24)){clock_interval=clock_interval?clock_interval*.85f+delta*.15f:delta;patch.tempo=bound((int)(MD_RATE*60/(24*clock_interval)+.5f),30,220);}
   clock_age=0;if(clock_pulse==0){untilstep=0;tick();}clock_pulse=(clock_pulse+1)%6;}
  return;
 }
 const int ch=(int)(st&15)+1;if(ch==16&&(st&240)==0xb0&&a>=16&&a<=19){apply({MD_MACRO,(int)a-16,(int)b*1000/127});return;}
 for(unsigned i=0;i<6;i++)if(in_chan[i]==ch)midi_track(i,st&240,a,b); // every track listening on this channel
}
static void midi_track(unsigned i,unsigned type,unsigned a,unsigned b){
 Voice &v=voices[i];
 if(type==0x90&&b){float note=midi_quantize?quantize_any((float)a,patch.root,patch.scale,0):(float)bound(a,24,96);fire(i,note,b/127.0f);held[i]=a;v.midi_gate=md_has_envelope(v.model)?1:0;}
 else if(type==0x80||(type==0x90&&!b)){if(held[i]==(int)a){held[i]=-1;if(!sustain[i])v.midi_gate=-1;}}
 else if(type==0xc0){unsigned id=midi_bank[i]*128+a;if(id<MD_TONE_PRESETS)tone_preset(i,id);}
 else if(type==0xe0)bend[i]=((int)(a|(b<<7))-8192)*(2.0f/8192);
 else if(type==0xb0){int value=b*1000/127;
  if(a==0)midi_bank[i]=b;
  else if(a==32){} /* Only LSB zero supported; reserved, never a send. */
  else if(a==121){expression[i]=1;bend[i]=0;sustain[i]=0;if(held[i]<0)v.midi_gate=-1;}
  else if(a==64){sustain[i]=b>=64;if(!sustain[i]&&held[i]<0)v.midi_gate=-1;}
  else if(a==120)reset_voice(i);
  else if(a==123){held[i]=-1;sustain[i]=0;v.midi_gate=-1;}
  else if(a==11)expression[i]=b/127.0f;
  else {int c=a==7?MD_LEVEL:a==10?MD_PAN:a==74?MD_CUTOFF:a==71?MD_RESONANCE:a==91?MD_REVERB_SEND:a==12?MD_DELAY_SEND:a==13?MD_SEND:a==5?MD_GLIDE:-1;if(c>=0)apply({c,(int)i,value});}
 }
}
unsigned md_command_space(void){return 64-(qwrite-qread);}
int md_command(int c,int t,int v) {if(c<0||c>=MD_COMMAND_COUNT)return 0;unsigned w=qwrite;if(w-qread>=64){stats.dropped_commands++;return 0;}queue[w&63]={c,t,v};fence();qwrite=w+1;return 1;}
void md_render(float *lr,unsigned n) {
 while(n){if(qread!=qwrite){while(qread!=qwrite){fence();Command e=queue[qread&63];apply(e);fence();qread++;}returnable=memcmp(&previous,&patch,sizeof patch)!=0;orbit_prepared=-1;}
  unsigned z=n<Q?n:Q;
  if(playing){
   if(!clock_source&&untilstep<=0)tick();
   for(unsigned i=0;i<6;i++)if(repeats[i].remaining&&repeats[i].left<=0){
    if(!patch.track[i].mute){fire(i,repeats[i].note,repeats[i].velocity);out_note(i,repeats[i].note,repeats[i].velocity,repeats[i].spacing*.5f);}
    repeats[i].remaining--;repeats[i].left+=repeats[i].spacing;
   }
   unsigned to=(!clock_source&&untilstep>0)?(unsigned)ceilf(untilstep):0;if(to&&z>to)z=to;
   for(unsigned i=0;i<6;i++)if(repeats[i].remaining){to=(unsigned)ceilf(repeats[i].left);if(to&&z>to)z=to;}
   if(routing.clock&&!clock_source){if(clock_left<=0){out_push(0xf8,0,0);clock_left+=MD_RATE*60.0f/(patch.tempo*24.0f);}to=(unsigned)ceilf(clock_left);if(to&&z>to)z=to;clock_left-=z;}
   untilstep-=z;for(unsigned i=0;i<6;i++)if(repeats[i].remaining)repeats[i].left-=z;
  }
  out_advance(z);
  if(clock_source&&playing){clock_age+=z;if(clock_age>MD_RATE*2){playing=0;memset(repeats,0,sizeof repeats);out_all_off();}}
  if(playing)orbit_run(); // Rhizome: gates crossed up to here fire before the voices render
  orbit_comet_follow();
  quantum(lr,z);lr+=z*2;n-=z;if(playing)orbit_since+=z;
 }
}
const md_patch *md_state(){return &patch;}const md_stats *md_status(){return &stats;}int md_playing(){return playing;}
int md_validate(const md_patch *p) {
 if(!p||p->magic!=0x4D445231||p->version!=13||!p->seed||p->tempo<30||p->tempo>220)return 0;
 if(p->biome_id< -1||p->biome_id>=MD_BIOMES||p->edited_mask>127)return 0;
 for(int i=0;i<6;i++)if(p->tone_preset[i]< -1||p->tone_preset[i]>=MD_TONE_PRESETS)return 0;
 for(int j=0;j<4;j++)if(p->macro[j]<0||p->macro[j]>1000)return 0;
 if(p->locked<0||p->locked>1||p->freeze<0||p->freeze>1||p->quality<0||p->quality>1)return 0;
 if(p->grain_size<0||p->grain_size>1000||p->grain_pos<0||p->grain_pos>1000||p->feedback<0||p->feedback>850)return 0;
 if(p->root<0||p->root>11||p->scale<0||p->scale>=SCALE_COUNT||p->transpose< -24||p->transpose>24||p->swing<0||p->swing>600||p->grain_pitch< -24||p->grain_pitch>24||p->reverse<0||p->reverse>1||p->duck_source< -1||p->duck_source>5||p->duck_amount<0||p->duck_amount>1000||p->duck_release<0||p->duck_release>1000)return 0;
 for(int i=0;i<6;i++){const md_track &t=p->track[i];if(t.attack<0||t.attack>1000||t.ratchet<1||t.ratchet>4||t.accent<0||t.accent>1000)return 0;if(t.model<0||t.model>=MD_MODELS||t.length<1||t.length>32||t.hits<0||t.hits>t.length||t.rotate<0||t.rotate>=t.length||t.note<24||t.note>96||t.mute<0||t.mute>1||t.speed<1||t.speed>8)return 0;
 if(t.filter<0||t.filter>=MD_FILTER_MODES)return 0;const int q[]={t.tone,t.harm,t.decay,t.level,t.send,t.shape,t.cutoff,t.resonance,t.breathe};for(int j=0;j<9;j++)if(q[j]<0||q[j]>1000)return 0;if(t.prob<0||t.prob>1000||t.pan<0||t.pan>1000)return 0;}
 for(int i=0;i<6;i++){const md_extension &e=p->ext[i];const int values[]={e.delay_send,e.reverb_send,e.glide,e.marbles,e.tides,e.tides_rate,e.warps};for(int v:values)if(v<0||v>1000)return 0;if(e.bass_mode<0||e.bass_mode>2||e.pattern<0||e.pattern>3||e.edit_step<0||e.edit_step>15)return 0;for(int v:e.notes)if(v< -1||v>24)return 0;}
 if(p->delay_ms<40||p->delay_ms>MD_TAPE_MS_MAX||p->delay_fb<0||p->delay_fb>800||p->reverb_time<0||p->reverb_time>1000||p->fx_level<0||p->fx_level>1000)return 0;
 if(p->send_hp<0||p->send_hp>1000||p->echo_tone<0||p->echo_tone>1000||p->room_tone<0||p->room_tone>1000)return 0;
 for(int i=0;i<6;i++){const md_gen &g=p->gen[i];if(g.source<0||g.source>=MD_GEN_SOURCES||g.dejavu<0||g.dejavu>1000||g.loop<1||g.loop>16||g.spread<0||g.spread>24)return 0;}
 for(int i=0;i<6;i++)if(p->gamaka[i]<0||p->gamaka[i]>=MD_GAMAKAS||p->gamaka_amount[i]<0||p->gamaka_amount[i]>1000)return 0;
 if(p->motion_every<0||p->motion_every>4)return 0;
 for(int i=0;i<6;i++){const md_orbit &o=p->orbit[i];if(o.gates<0||o.gates>MD_ORBIT_GATES_MAX||o.period<0||o.period>=MD_ORBIT_PERIODS||o.ecc<0||o.ecc>MD_ORBIT_ECC_MAX||o.phase<0||o.phase>=MD_ORBIT_PHASES||o.pitch<0||o.pitch>=MD_PITCH_SOURCES||
  o.moons<0||o.moons>MD_MOONS_MAX||o.moon_spin<0||o.moon_spin>=MD_MOON_SPINS||o.moon_interval<0||o.moon_interval>=MD_MOON_INTERVALS||o.moon_level<0||o.moon_level>100)return 0;}
 if(p->orbit_comet<0||p->orbit_comet>=MD_COMETS||p->comet_event<0||p->comet_event>=MD_COMET_EVENTS||p->comet_amount<0||p->comet_amount>100||p->orbit_conj<0||p->orbit_conj>=MD_CONJ_MODES||p->orbit_system<0||p->orbit_system>=MD_ORBIT_SYSTEMS)return 0;
 if(p->tape_heads<1||p->tape_heads>7||p->tape_wow<0||p->tape_wow>1000||p->tape_drive<0||p->tape_drive>1000||p->tape_age<0||p->tape_age>1000)return 0;
 if(p->hall_size<0||p->hall_size>1000||p->hall_predelay<0||p->hall_predelay>250||p->hall_bass<0||p->hall_bass>1000||p->hall_mod<0||p->hall_mod>1000||p->hall_vintage<0||p->hall_vintage>1)return 0;
 if(p->beads_shape<0||p->beads_shape>1000||p->beads_density<0||p->beads_density>1000||p->beads_random<0||p->beads_random>1000||p->beads_media<0||p->beads_media>=MD_BEADS_MEDIAS)return 0;
 if(p->orbit_prec<0||p->orbit_prec>100||p->orbit_pert<0||p->orbit_pert>100)return 0;
 if(p->vadi< -1||p->vadi>11||p->samvadi< -1||p->samvadi>11)return 0;
 if(p->form<0||p->form>1000||p->form_bars<0||p->form_bars>3||p->form_drops<0||p->form_drops>3)return 0;
 if(p->echo_sync<0||p->echo_sync>=MD_ECHO_SYNCS||p->room_diffusion<0||p->room_diffusion>1000||p->chorus<0||p->chorus>1000||p->chorus_depth<0||p->chorus_depth>1000)return 0;
 for(int i=0;i<6;i++)for(int s=0;s<2;s++){const md_lfo &l=p->lfo[i][s];if(l.rate<0||l.rate>=MD_LFO_RATES||l.shape<0||l.shape>=MD_LFO_SHAPES||l.target<0||l.target>=MD_LFO_TARGETS||l.depth< -1000||l.depth>1000)return 0;}
 return 1;
}
int md_load(const md_patch *p){if(playing||!md_validate(p))return 0;patch=*p;orbit_prepared=-1;previous=patch;returnable=0;morph_on=0;drift_on=0;drift_slow=0;morph_span=0;fx_shed=0;seqrng=patch.seed;clear_audio();for(int j=0;j<4;j++)smooth[j]=unit(patch.macro[j]);for(int i=0;i<6;i++)reset_voice(i);mark_saved();return 1;}
unsigned md_memory_bytes(){return sizeof drift_home+sizeof morph_before+sizeof morph_a+sizeof morph_b+sizeof morph_kind+sizeof chorus_line+64+sizeof hall_buf+sizeof sixop_bank+sizeof element_body+sizeof element_strike+sizeof gr_tape+sizeof reel+sizeof reel_decode+sizeof reel_trace+sizeof voices_raw+sizeof patch+sizeof previous+sizeof stats+sizeof queue+sizeof repeats+512;}
int md_decode(md_patch *out,const void *data,unsigned bytes){
 if(!out||!data)return 0;
 if(bytes==sizeof(md_patch)){md_patch p;memcpy(&p,data,sizeof p);if(p.version==12){rhizome_v12_voices(p);p.version=13;p.biome_id=-1;}else if(p.version==11){p.version=13;p.biome_id=-1;}if(!md_validate(&p))return 0;*out=p;return 1;}
 // Older patches (Monades, Mutable Dreams): their Monde numbers mean nothing here, so they come back as your own.
 // Monades v10 (1680 bytes): no orbits.
 if(bytes==offsetof(md_patch,orbit)){md_patch p;memcpy(&p,data,bytes);if(p.version!=10)return 0;v11_defaults(p);p.biome_id=-1;if(!md_validate(&p))return 0;*out=p;return 1;}
 // Monades v9 (1676 bytes): Motion varies every step.
 if(bytes==offsetof(md_patch,motion_every)){md_patch p;memcpy(&p,data,bytes);if(p.version!=9)return 0;v10_defaults(p);p.biome_id=-1;if(!md_validate(&p))return 0;*out=p;return 1;}
 // Monades v8 (1620 bytes): no gamakas, no pivot notes.
 if(bytes==offsetof(md_patch,gamaka)){md_patch p;memcpy(&p,data,bytes);if(p.version!=8)return 0;v9_defaults(p);p.biome_id=-1;if(!md_validate(&p))return 0;*out=p;return 1;}
 // Monades v7 (1512 bytes): generators and form start off.
 if(bytes==offsetof(md_patch,gen)){md_patch p;memcpy(&p,data,bytes);if(p.version!=7)return 0;v8_defaults(p);p.biome_id=-1;if(!md_validate(&p))return 0;*out=p;return 1;}
 // Mutable Dreams 1.1 (v5 / v6, 1292 bytes): everything Monades added starts neutral.
 if(bytes==offsetof(md_patch,send_hp)){md_patch p;memcpy(&p,data,bytes);if(p.version!=5&&p.version!=6)return 0;v7_defaults(p);p.biome_id=-1;if(!md_validate(&p))return 0;*out=p;return 1;}
 struct OldTrack{int32_t values[14];};
 struct OldPatch{uint32_t magic,version,seed;int32_t macro[4],tempo,locked,freeze,grain_size,grain_pos,feedback,quality;OldTrack track[6];};
 struct V2Track{int32_t values[17];};
 struct V2Patch{uint32_t magic,version,seed;int32_t macro[4],tempo,locked,freeze,grain_size,grain_pos,feedback,quality;V2Track track[6];int32_t globals[9];};
 md_patch p={};
 if(bytes==652){memcpy(&p,data,652);if(p.version!=4)return 0;extension_defaults(p);p.biome_id=-1;if(!md_validate(&p))return 0;*out=p;return 1;}
 if(bytes==620){
  memcpy(&p,data,620);if(p.version!=3)return 0;
  extension_defaults(p);p.biome_id=-1;p.edited_mask=1;for(int i=0;i<6;i++)p.tone_preset[i]=-1;
  if(!md_validate(&p))return 0;*out=p;return 1;
 }
 if(bytes==sizeof(V2Patch)){
  V2Patch old;memcpy(&old,data,sizeof old);if(old.version!=2)return 0;
  memcpy(&p,&old,offsetof(V2Patch,track));memcpy(&p.root,old.globals,sizeof old.globals);
  for(int i=0;i<6;i++)memcpy(&p.track[i],&old.track[i],sizeof(V2Track));
 }else if(bytes==sizeof(OldPatch)){
  OldPatch old;memcpy(&old,data,sizeof old);if(old.version!=1)return 0;
  memcpy(&p,&old,offsetof(OldPatch,track));p.scale=1;p.duck_source=-1;p.duck_release=350;
  for(int i=0;i<6;i++){memcpy(&p.track[i],&old.track[i],sizeof(OldTrack));p.track[i].ratchet=1;}
 }else return 0;
 extension_defaults(p);p.biome_id=-1;p.edited_mask=1;for(int i=0;i<6;i++)p.tone_preset[i]=-1;
 for(int i=0;i<6;i++){p.track[i].shape=p.track[i].decay;p.track[i].cutoff=850;}
 if(!md_validate(&p))return 0;*out=p;return 1;
}
int md_import(const void *data,unsigned bytes){
 if(playing)return 0;md_patch decoded;return md_decode(&decoded,data,bytes)&&md_load(&decoded);
}
const char *md_biome_name(int id){return id<0?"USER MONDE":biome_bank[bound(id,0,MD_BIOMES-1)].name;}
const char *md_biome_family(int id){return id<0?"CUSTOM":biome_families[biome_bank[bound(id,0,MD_BIOMES-1)].family];}
const char *md_tone_name(int id){return id<0?"MONDE VOICE":tone_bank[bound(id,0,MD_TONE_PRESETS-1)].name;}
int md_tone_model(int id){return tone_bank[bound(id,0,MD_TONE_PRESETS-1)].model;}
int md_has_envelope(int model){return model==MD_FM||model==MD_VA||model==MD_HARMONIC||model==MD_CHOIR||model==MD_BASS||model==MD_MELLOW||model==MD_SATIN||model==MD_SIXOP||model==MD_STRINGS;}
int md_cutoff_hz(int value){return (int)cutoff_hz(unit(value));}

unsigned md_scale_mask(int scale){return scale_mask_for(scale,0);}
unsigned md_scale_mask_down(int scale){return scale_mask_for(scale,1);}
int md_quantize(int note,int root,int scale){return quantize_mask(note,root,scale_mask_for(scale,0));}
const char *md_scale_name(int s){s=bound(s,0,SCALE_COUNT-1);return s>=SCALE_TWELVE?micro_defs[s-SCALE_TWELVE].name:scale_defs[s].name;}
const char *md_scale_family(int s){s=bound(s,0,SCALE_COUNT-1);return scale_families[s>=SCALE_TWELVE?micro_defs[s-SCALE_TWELVE].family:scale_defs[s].family];}
float md_quantize_pitch(float note,int root,int scale){return quantize_any(note,root,scale,0);}
int md_scale_cents(int s,float *degrees,int max,float *period){
 // Degrees in cents above the root (the root first); twelve-tone scales give their ascent set.
 s=bound(s,0,SCALE_COUNT-1);int n=0;
 if(s>=SCALE_TWELVE){const MicroDef &m=micro_defs[s-SCALE_TWELVE];*period=m.period;if(n<max)degrees[n++]=0;for(unsigned k=0;k<m.count&&n<max;k++)degrees[n++]=m.degree[k];return n;}
 unsigned mask=scale_mask_for(s,0);*period=1200;for(int k=0;k<12&&n<max;k++)if(mask&(1u<<k))degrees[n++]=k*100.0f;return n;
}
int md_scale_count(void){return SCALE_COUNT;}
int md_scale_custom(void){return SCALE_CUSTOM;}
int md_scale_at(int position){return scale_order[bound(position,0,SCALE_COUNT-1)];}
int md_scale_position(int scale){for(int k=0;k<SCALE_COUNT;k++)if(scale_order[k]==scale)return k;return 0;}
float md_lfo_wave(int track,int slot,uint32_t cycle,float phase){track=bound(track,0,5);slot=bound(slot,0,1);return lfo_shape(patch.lfo[track][slot].shape,track,slot,cycle,phase-floorf(phase));}
float md_lfo_sixteenths(int rate){return rate>=0&&rate<MD_LFO_SYNCED?lfo_sync[rate]:0;}
float md_lfo_hz(int rate,int tempo){rate=bound(rate,0,MD_LFO_RATES-1);return rate<MD_LFO_SYNCED?tempo/15.0f/lfo_sync[rate]:lfo_free_hz(rate);}
const char *md_lfo_rate_name(int rate){rate=bound(rate,0,MD_LFO_RATES-1);return rate<MD_LFO_SYNCED?lfo_sync_names[rate]:"FREE";}
const char *md_lfo_shape_name(int s){static const char *const n[MD_LFO_SHAPES]={"SINE","TRIANGLE","RAMP UP","RAMP DOWN","SQUARE","SAMPLE & HOLD","SMOOTH RANDOM"};return n[bound(s,0,MD_LFO_SHAPES-1)];}
const char *md_lfo_target_name(int t){static const char *const n[MD_LFO_TARGETS]={"CUTOFF","RESONANCE","LEVEL","PAN","PITCH","TONE","HARMONICS","SHAPE","ECHO SEND","ROOM SEND","GRAIN SEND","ECHO TIME","GRAIN POS","ATTACK","DECAY","ECHO FEEDBACK","GRAIN SIZE","NOTES IN SCALE","HITS","ROTATE"};return n[bound(t,0,MD_LFO_TARGETS-1)];}
float md_echo_ms(void){return echo_ms_now();}
void md_hall_view(md_hall *v){
 hall_rt60s(v->rt60);for(int k=0;k<3;k++)v->energy[k]=hall_env[k]*HALL_OUT*HALL_OUT;v->predelay_ms=(float)patch.hall_predelay;v->size=hall_set.size;v->vintage=hall_set.vintage;v->infinite=v->rt60[1]<0;
}
float md_hall_rt60(int decay){return hall_rt60(bound(decay,0,1000));}
void md_reel_view(md_reel *v){
 v->speed=reel_speed*reel_mod;v->wow=reel_wow_now*patch.tape_wow*.001f;v->path=REEL_PATH;v->pos=(uint32_t)reel_pos;v->length=REEL_N;v->heads=reel_heads();v->transport=reel_transport;
 for(int k=0;k<3;k++){v->head_peak[k]=reel_head_peak[k];reel_head_peak[k]*=.5f;}v->rec_peak=reel_rec_peak;reel_rec_peak*=.5f;memcpy(v->trace,reel_trace,sizeof v->trace);
}
int md_morph_bars_len(int bars){return morph_len(bars);}
int md_drift_bars(int slow){return drift_len(slow);}
void md_drift_view(md_drift_state *s,const md_patch **home){
 s->on=drift_on;s->slow=drift_slow;s->amount=drift_amount;s->voyage=drift_voyage;s->scope=drift_scope;s->pos=drift_on&&morph_on?morph_pos:0;s->distance=drift_distance;
 s->seconds=drift_seconds;const int span=drift_len(drift_slow)*16;s->next=drift_on?(1000-s->pos)*.001f*span*60.0f/(patch.tempo*4.0f):0;
 memcpy(s->events,drift_log,sizeof drift_log);if(home)*home=&drift_home;
}
int md_morph_point(int track){return morph_point(track);}
unsigned md_morph_changes(void){return morph_changes;}
void md_morph_view(md_morph_state *s,const md_patch **a,const md_patch **b){
 s->on=morph_on;s->target=morph_target;s->pos=morph_pos;s->bars=morph_bars;s->scope=morph_scope;if(a)*a=&morph_a;if(b)*b=&morph_b;
}
int md_can_return(void){return returnable;}
int md_unsaved(void){uint32_t h[2];patch_hash(h);return h[0]!=saved_hash[0]||h[1]!=saved_hash[1]||(!clock_source&&patch.tempo!=saved_tempo);}
void md_mark_saved(void){mark_saved();}
unsigned md_form_mask(unsigned c,unsigned s){return form_mask(c,s);}
float md_form_tension(unsigned s){return form_tension_of[s&3];}
int md_form_bars(void){return (int)form_bar_len[bound(patch.form_bars,0,3)];}
const char *md_gamaka_name(int s){static const char *const n[MD_GAMAKAS]={"OFF","KAN","MEEND","ANDOLAN","MIX"};return n[bound(s,0,MD_GAMAKAS-1)];}
float md_gamaka_shape(int style,int amount,float from,float ms){return gamaka_shape(style,amount,from,ms);}
const char *md_gen_source_name(int s){static const char *const n[MD_GEN_SOURCES]={"OFF","RANDOM","MARKOV","MOTIF"};return n[bound(s,0,MD_GEN_SOURCES-1)];}
float md_echo_sync_len16(int s){return echo_sync_len[bound(s,0,MD_ECHO_SYNCS-1)];}
const char *md_echo_sync_name(int s){static const char *const n[MD_ECHO_SYNCS]={"OFF","1/32","1/16 T","1/16","1/8 T","1/16 .","1/8","1/4 T","1/8 .","1/4","1/4 .","1/2","1/2 ."};return n[bound(s,0,MD_ECHO_SYNCS-1)];}
int md_send_hp_hz(int v){return v>0?(int)(20.0f*stmlib::SemitonesToRatio(bound(v,0,1000)*.0797262f)+.5f):0;}
void md_set_custom_scale(unsigned mask){custom_scale=(uint16_t)((mask&0xfff)|1u);}
const char *md_model_name(int m){static const char *names[]={"PLAITS KICK","PLAITS SNARE","PLAITS HAT","PLAITS FM","PLAITS VA","RINGS MODAL","PLAITS HARMONIC","SINE CHOIR","ROUND BASS","ELEMENTS STRIKE","MELLOW DRIFT","SATIN MORPH","SIX-OP FM","STRING MACHINE"};return names[bound(m,0,MD_MODELS-1)];}
unsigned md_voice_arena_spare(){return (unsigned)arena_spare;}
// Rhizome's two voices name their knobs (Plaits' TIMBRE, HARMONICS, MORPH); Monades' keep TONE, HARMONICS, SHAPE.
const char *md_knob_name(int m,int k){static const char *const sixop[3]={"BRIGHT","SOUND","ENVELOPE"},*const strings[3]={"BRIGHT","CHORD","REGISTER"},*const plain[3]={"TONE","HARMONICS","SHAPE"};
 return (m==MD_SIXOP?sixop:m==MD_STRINGS?strings:plain)[bound(k,0,2)];}
const char *md_harm_name(int m,int harm){
 if(m==MD_STRINGS){static const char *const chords[11]={"OCTAVE","FIFTH","SUS4","MINOR","MINOR 7","MINOR 9","MINOR 11","6/9","MAJOR 9","MAJOR 7","MAJOR"};
  return chords[bound((int)(harm*.001f*1.02f*11),0,10)];} // as Plaits' chord quantizer, without its hysteresis
 if(m==MD_SIXOP){static char name[11];const int k=bound((int)(harm*.001f*1.02f*96),0,95);static const uint8_t *const banks[3]={plaits::syx_bank_0,plaits::syx_bank_1,plaits::syx_bank_2};
  const uint8_t *at=banks[k/32]+(k%32)*plaits::fm::Patch::SYX_SIZE+118;int n=0;for(int c=0;c<10;c++){char x=(char)(at[c]&0x7f);if(x>='a'&&x<='z')x-=32;if(x==' '&&n&&name[n-1]==' ')continue;name[n++]=x;}
  while(n&&name[n-1]==' ')n--;name[n]=0;return name;}
 return 0;
}
}
