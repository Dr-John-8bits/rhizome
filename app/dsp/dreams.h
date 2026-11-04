/* SPDX-License-Identifier: MIT OR GPL-3.0-only */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { MD_TRACKS=6, MD_MODELS=14, MD_RATE=44100, MD_QUANTUM=64, MD_BIOMES=22, MD_TONE_PRESETS=256, MD_BLANK_BIOME=21 }; /* Rhizome: 21 Mondes and EMPTY SKY */
/* Monades' twelve voices, then Rhizome's two own, from Plaits (Mutable Instruments, MIT): SIX-OP FM (K2 picks one of
 * 96 DX7 sounds) and STRING MACHINE (K2 the chord). For these two, K1 TONE is Plaits' TIMBRE, K3 SHAPE its MORPH. */
enum { MD_KICK, MD_SNARE, MD_HAT, MD_FM, MD_VA, MD_RINGS, MD_HARMONIC, MD_CHOIR, MD_BASS, MD_ELEMENTS, MD_MELLOW, MD_SATIN, MD_SIXOP, MD_STRINGS };
enum { MD_MODEL, MD_TONE, MD_HARM, MD_DECAY, MD_LEVEL, MD_SEND, MD_HITS,
 MD_LENGTH, MD_ROTATE, MD_PROB, MD_NOTE, MD_PAN, MD_MUTE, MD_SPEED,
 MD_MACRO, MD_TEMPO, MD_PLAY, MD_LOCK, MD_MUTATE, MD_RETURN, MD_SCENE,
 MD_FREEZE, MD_GRAIN_SIZE, MD_GRAIN_POS, MD_FEEDBACK, MD_QUALITY, MD_TRIGGER, MD_PANIC,
 MD_ATTACK,MD_SWING,MD_RATCHET,MD_ROOT,MD_SCALE,MD_TRANSPOSE,MD_GRAIN_PITCH,
 MD_REVERSE,MD_DUCK_SOURCE,MD_DUCK,MD_DUCK_RELEASE,MD_ACCENT,MD_SHAPE,MD_CUTOFF,MD_RESONANCE,MD_FILTER,MD_BREATHE,MD_TONE_PRESET,MD_DELAY_SEND,MD_REVERB_SEND,MD_DELAY_TIME,MD_DELAY_FB,MD_REVERB_TIME,MD_FX_LEVEL,MD_BASS_MODE,MD_BASS_PATTERN,MD_GLIDE,MD_MARBLES,MD_TIDES,MD_TIDES_RATE,MD_WARPS,MD_BASS_STEP,MD_BASS_VALUE,MD_CLOCK_SOURCE,MD_MIDI_QUANTIZE,
 MD_LFO_RATE,MD_LFO_SHAPE,MD_LFO_TARGET,MD_LFO_DEPTH,MD_SEND_HP,MD_ECHO_TONE,MD_ROOM_TONE,
 MD_ECHO_SYNC,MD_ROOM_DIFFUSION,MD_CHORUS,MD_CHORUS_DEPTH,
 MD_GEN_SOURCE,MD_GEN_DEJAVU,MD_GEN_LOOP,MD_GEN_SPREAD,MD_FORM,MD_FORM_BARS,MD_FORM_DROPS,
 MD_GAMAKA,MD_GAMAKA_AMOUNT,MD_VADI,MD_SAMVADI,MD_MOTION_EVERY,MD_CLEAR_PHRASE,MD_DICE,
 MD_MORPH_TO,MD_MORPH,MD_MORPH_BARS,MD_MORPH_SCOPE,MD_MORPH_KEEP,
 MD_DRIFT_SLOW,MD_DRIFT_AMOUNT,MD_DRIFT_VOYAGE,MD_DRIFT_SCOPE,MD_DRIFT_HOME,
 MD_OVERLOAD,
 MD_ORBIT_GATES=120,MD_ORBIT_PERIOD,MD_ORBIT_ECC,MD_ORBIT_PHASE,MD_ORBIT_PITCH,MD_ORBIT_PREC,MD_ORBIT_PERT,
 MD_ORBIT_MOONS,MD_ORBIT_MOON_SPIN,MD_ORBIT_MOON_INTERVAL,MD_ORBIT_MOON_LEVEL,
 MD_ORBIT_COMET,MD_ORBIT_COMET_EVENT,MD_ORBIT_COMET_AMOUNT,MD_ORBIT_CONJ,MD_ORBIT_SYSTEM,
 MD_TAPE_HEADS,MD_TAPE_WOW,MD_TAPE_DRIVE,MD_TAPE_AGE,MD_TAPE_TRANSPORT,
 MD_HALL_SIZE,MD_HALL_PREDELAY,MD_HALL_BASS,MD_HALL_MOD,MD_HALL_VINTAGE,
 MD_BEADS_SHAPE,MD_BEADS_DENSITY,MD_BEADS_RANDOM,MD_BEADS_MEDIA, /* Rhizome: from 120, after OVERLOAD (the earlier numbers stay) and past 100..116, which the browser and the editor keep for their own codes (banks, preferences) */
 MD_COMMAND_COUNT }; /* OVERLOAD: the audio ran late; FULL drops to ECO, ECO sheds chorus and room diffusion until the next Monde */ /* CLEAR_PHRASE: all 16 steps rest, cursor on step 1 */ /* LFO commands: track argument = track + 6 * slot (A, B) */
typedef struct { int32_t model,tone,harm,decay,level,send,hits,length,rotate,prob,note,pan,mute,speed,attack,ratchet,accent,shape,cutoff,resonance,filter,breathe; } md_track;
typedef struct { int32_t delay_send,reverb_send,bass_mode,pattern,glide,marbles,tides,tides_rate,warps,edit_step; int32_t notes[16]; } md_extension;
/* Monades LFO, two per track. rate: 0..14 synced to the tempo (8 bars .. 1/32), 15..30 free 0.02..20 Hz.
 * depth -1000..1000 (0 = off, negative inverts). Filter modes: 0 off, 1 LP12, 2 LP24, 3 LPG, 4 HP12, 5 BP, 6 notch, 7 HP24.
 * echo_sync: 0 = Echo ms, 1..9 = 1/32 .. 1/4 of the tempo (halved until it fits the 350 ms line). */
enum { MD_LFO_SYNCED=15, MD_LFO_RATES=31, MD_LFO_SHAPES=7, MD_LFO_TARGETS=20, MD_FILTER_MODES=8, MD_ECHO_SYNCS=13 };
enum { MD_LFO_SINE, MD_LFO_TRIANGLE, MD_LFO_RAMP_UP, MD_LFO_RAMP_DOWN, MD_LFO_SQUARE, MD_LFO_SAMPLE_HOLD, MD_LFO_SMOOTH_RANDOM };
enum { MD_TO_CUTOFF, MD_TO_RESONANCE, MD_TO_LEVEL, MD_TO_PAN, MD_TO_PITCH, MD_TO_TONE, MD_TO_HARMONICS, MD_TO_SHAPE,
 MD_TO_ECHO_SEND, MD_TO_ROOM_SEND, MD_TO_GRAIN_SEND, MD_TO_ECHO_TIME, MD_TO_GRAIN_POS,
 MD_TO_ATTACK, MD_TO_DECAY, MD_TO_ECHO_FEEDBACK, MD_TO_GRAIN_SIZE, MD_TO_NOTES, MD_TO_HITS, MD_TO_ROTATE }; /* NOTES: scale steps on generated notes; HITS: density; ROTATE: steps the pattern turns (full depth = a whole turn) */
typedef struct { int32_t rate,shape,target,depth; } md_lfo;
/* Monades generator, per track. source: 0 off (Mutable Dreams), 1 random within spread semitones, 2 Markov chain
 * learned from the track's phrase. dejavu 0..1000: chance to replay the loop register instead of drawing anew
 * (1000 = frozen loop; notes and Chance decisions alike). loop 1..16 hits. */
enum { MD_GEN_OFF, MD_GEN_RANDOM, MD_GEN_MARKOV, MD_GEN_MOTIF, MD_GEN_SOURCES, MD_FORM_SECTIONS=4 }; /* MOTIF: walks the phrase in order, spread = variation */
/* Gamakas (raga ornaments) on melodic tracks: kan = grace from the upper scale note, meend = glide from the previous
 * note, andolan = slow sway towards the lower scale note, mix = one of them per note, chosen from the seed. */
/* Dice (track, page): new values for what that page's K1-K4 set, drawn within musical ranges. Scale, root, transpose,
 * tempo, mutes, engines and MIDI never change; a track's level changes only on the mixer. ENV undoes it like a variation. */
enum { MD_DICE_SOUND, MD_DICE_FILTER, MD_DICE_ENVELOPE, MD_DICE_MIX, MD_DICE_MIXER, MD_DICE_RHYTHM, MD_DICE_GROOVE, MD_DICE_BASS,
 MD_DICE_PHRASE, MD_DICE_FORM, MD_DICE_GRAIN, MD_DICE_TAPE, MD_DICE_SPACE, MD_DICE_SPACE_TONE, MD_DICE_ECHO, MD_DICE_MODULATION,
 MD_DICE_LFO_A, MD_DICE_LFO_B, MD_DICE_RAGA, MD_DICE_PAGES };
enum { MD_GAMAKA_OFF, MD_GAMAKA_KAN, MD_GAMAKA_MEEND, MD_GAMAKA_ANDOLAN, MD_GAMAKA_MIX, MD_GAMAKAS };
typedef struct { int32_t source,dejavu,loop,spread; } md_gen;
/* Rhizome orbits, per track. gates 0 = off (the track keeps its euclidean rhythm), 1..16 = a body crossing that many
 * gates per turn, spread as a euclidean rhythm on the turn's steps. period: index of md_orbit_steps (one turn in
 * sixteenths, 4 to 192; then eight free periods, 5.1 to 31.8 seconds whatever the tempo, which never fall together). ecc 0..850 thousandths (0 = round: the exact euclidean rhythm). phase: the gates turn by
 * 15 degree steps (0..23). pitch: where notes come from: the distance to the star, the track's phrase in order, or
 * the triad note above the previous track's last note (tintinnabuli).
 * Moons (0..3) circle the body: each note it plays spins them once around it, in moon_spin (index of md_orbit_moon_steps,
 * 1 to 16 sixteenths), and each moon plays an ornament as it passes the body's outer side, so the moons share the spin
 * evenly after the note. moon_interval: what they play from the note (the same note, scale steps up or down, thirds
 * up or down, or the harmonics octave, octave and fifth, two octaves). moon_level 0..100: their velocity, from the note's. */
enum { MD_ORBIT_PERIODS=21, MD_ORBIT_TEMPO_PERIODS=13, MD_ORBIT_PHASES=24, MD_ORBIT_ECC_MAX=850, MD_ORBIT_GATES_MAX=16, MD_MOONS_MAX=3, MD_MOON_SPINS=8 };
enum { MD_PITCH_ORBIT, MD_PITCH_MOTIF, MD_PITCH_ECHO, MD_PITCH_SOURCES };
/* The comet: one turn in 4 to 64 bars (orbit_comet 1..5, 0 = none), so eccentric that it spends most of it far away
 * and swings past the star in about a fifth of it. While it passes, the event (comet_event): the room swells (a longer
 * tail, more return, more send), the granular tape freezes, or both, or FILTER: a low-pass on the whole mix, closed
 * while the comet is far and opening wide as it grazes the star (comet_amount: how far it closes, down to 280 Hz);
 * comet_amount 0..100 is how much.
 * orbit_comet 6, WANDER: it comes back at random, every 4 to 32 bars (drawn from the seed), and at each passage shocks
 * the body nearest its path: for a few of its notes (comet_amount: how many, how far), a note skipped, the body pushed
 * on by a step, or the note moved by scale steps; the shock may pass to the next body, less likely at each link. It is
 * all temporary: nothing is written into the patch. Each shock also jolts the effects: the HALL swells and the tape
 * slows for a moment (about 1.5 s, as strong as comet_amount).
 * orbit_conj: what a conjunction does: nothing, accent both bodies' next beat, or strike them together as a chord. */
enum { MD_COMETS=7, MD_COMET_WANDER=6, MD_COMET_SWELL=0, MD_COMET_FREEZE, MD_COMET_BOTH, MD_COMET_FILTER, MD_COMET_EVENTS };
enum { MD_CONJ_OFF, MD_CONJ_ACCENT, MD_CONJ_CHORD, MD_CONJ_MODES };
/* Real systems (orbit_system 1..4, 0 = your own): MD_ORBIT_SYSTEM loads one onto the tracks (unmuted melodic ones
 * first): measured period ratios and eccentricities, the same number of gates on each body so the ratios are heard as
 * a polyrhythm. Any later change to an orbit makes the system your own again (0). ENV undoes the load. */
enum { MD_ORBIT_SYSTEMS=5 };
/* Rhizome's tape echo (in place of the echo; delay_ms, delay_fb, echo_sync, echo_tone and the sends keep their roles).
 * tape_heads: which of the three playback heads play (bits 1, 2, 4: a third, two thirds, all of the path); delay_ms
 * (40..2860) is the time of the longest one, reached by the tape's speed (0.5 to 12 times the slowest). tape_wow, tape_drive, tape_age 0..1000.
 * MD_TAPE_TRANSPORT (not stored): RUN, FREEZE (the loop repeats, nothing recorded), STOP (the tape brakes), REWIND (the
 * record head lifts and the tape runs back three times as fast: the echoes play reversed, higher). */
enum { MD_TAPE_RUN, MD_TAPE_FREEZE, MD_TAPE_STOP, MD_TAPE_REWIND, MD_TAPE_TRANSPORTS, MD_TAPE_MS_MAX=2860 };
typedef struct { float speed,wow,path,head_peak[3],rec_peak; uint32_t pos,length; int32_t heads,transport; uint8_t trace[128]; } md_reel;
void md_reel_view(md_reel *view);
/* Rhizome's HALL (in place of the room; reverb_time, room_tone, room_diffusion and the sends keep their roles):
 * reverb_time 0..999 = DECAY 0.5 s to 20 s, 1000 = infinite; room_tone = TREBLE (1.5 to 18 kHz); room_diffusion =
 * DIFFUSION. hall_size 0..1000, hall_predelay 0..250 ms, hall_bass 0..1000 (0.5x to 2x the decay, 500 = 1x),
 * hall_mod 0..1000, hall_vintage 0/1 (22.05 kHz). The view: RT60 of the low, middle and high tail (-1: infinite) and
 * the tail's energy in those bands now. */
typedef struct { float rt60[3],energy[3],predelay_ms,size; int32_t vintage,infinite; } md_hall;
void md_hall_view(md_hall *view);
float md_hall_rt60(int decay); /* seconds for a DECAY value, -1: infinite */
/* Rhizome's granular bus is Beads (Mutable Instruments): beads_density 0..1000 (below 480 regular grains, faster toward 0;
 * above 520 random grains, more toward 1000; between, none), beads_shape and beads_random 0..1000, beads_media
 * MD_BEADS_DIRTY_TAPE .. MD_BEADS_DIGITAL (the memory's grain and length). */
enum { MD_BEADS_DIRTY_TAPE, MD_BEADS_CLEAN_TAPE, MD_BEADS_DIGITAL, MD_BEADS_MEDIAS }; /* Beads' VINTAGE is left out (flash) */
float md_beads_seconds(void); /* the memory's length now, for the media in use */
float md_beads_rate(int density); /* new grains a second asked for (regular below the middle, an average above) */
float md_beads_envelope(float shape,float t); /* the grain envelope SHAPE gives, at t (0..1 of the grain) */ /* speed: 1 = the slowest tape that still gives 1.43 s on head 3 */
enum { MD_MOON_REPEAT, MD_MOON_UP, MD_MOON_DOWN, MD_MOON_THIRDS_UP, MD_MOON_THIRDS_DOWN, MD_MOON_HARMONICS, MD_MOON_INTERVALS };
typedef struct { int32_t gates,period,ecc,phase,pitch,moons,moon_spin,moon_interval,moon_level; } md_orbit;
/* The fields of a Monades v10 patch (1680 bytes), shared by md_patch and the factory Mondes' table, which keeps them
 * at that size: Rhizome's fields are set when a Monde loads, as for a Monades save. Monades' notes on them:
 * Monades v7: high-pass on the echo and room sends (0 = off), echo / room tone (500 = Mutable Dreams), LFOs.
 * chorus = wet amount on the dry mix, 0 = off
 * v8: generators, and the form: calm / build / peak / release of 2-16 bars (form_bars 0..3), depth 0 = off.
 * v9: gamaka style and amount per track; vadi / samvadi = semitones above the root, -1 = none.
 * v10: K2 Motion holds its variation for 1, 2, 4, 8 or 16 steps (0..4); 0 = every step, as 1.1
 */
#define MD_PATCH_V10_FIELDS \
 uint32_t magic,version,seed; int32_t macro[4],tempo,locked,freeze,grain_size,grain_pos,feedback,quality; \
 md_track track[MD_TRACKS]; \
 int32_t root,scale,transpose,swing,grain_pitch,reverse,duck_source,duck_amount,duck_release; \
 int32_t biome_id,tone_preset[6]; uint32_t edited_mask; \
 md_extension ext[6]; int32_t delay_ms,delay_fb,reverb_time,fx_level; \
 int32_t send_hp,echo_tone,room_tone; md_lfo lfo[6][2]; \
 int32_t echo_sync,room_diffusion,chorus,chorus_depth; \
 md_gen gen[6]; int32_t form,form_bars,form_drops; \
 int32_t gamaka[6],gamaka_amount[6],vadi,samvadi; \
 int32_t motion_every;
typedef struct { MD_PATCH_V10_FIELDS } md_patch_v10;
typedef struct {
 MD_PATCH_V10_FIELDS
 /* Rhizome v11: the orbits; precession 0..100 (100 = each ellipse turns once in 16 bars), perturbation 0..100. */
 md_orbit orbit[6]; int32_t orbit_prec,orbit_pert,orbit_comet,comet_event,comet_amount,orbit_conj,orbit_system,tape_heads,tape_wow,tape_drive,tape_age,hall_size,hall_predelay,hall_bass,hall_mod,hall_vintage,beads_shape,beads_density,beads_random,beads_media;
} md_patch;
typedef struct { uint32_t frames,step,events[6],dropped_commands,nonfinite; float peak; uint32_t grain_writes,voice_quanta,skipped_quanta; int32_t last_note[6]; uint32_t quality_reductions,element_modes; float track_peak[6];
 /* Monades meters, output-referred like the final mix: grain/delay/room returns, master L/R, pre-limiter peak. */
 float fx_peak[3],out_peak[2],limit_peak;
 /* Monades views: actual (modulated) filter cutoff in Hz and Q, envelope level and attack phase per track. */
 float cutoff_now[6],q_now[6],env_now[6]; int32_t env_attacking[6];
 float duck_gain,tide_phase[6]; /* ducking gain now (1 = none), Tides cycle position 0..1 per track */
 uint32_t step_played[6],step_skipped[6]; /* per track, bit = step of its cycle: last pass played / hit refused by Chance */
 uint32_t midi_out_notes,midi_out_dropped; /* Monades MIDI out: note-ons queued, messages lost to a full queue */
 float last_pitch[6]; /* last note fired per track, fractional for microtonal scales */
 float lfo_phase[6][2],lfo_value[6][2]; uint32_t lfo_cycle[6][2]; /* where each LFO is: phase 0..1 in cycle n, output -1..1 */
 int32_t gen_off[6][16]; uint32_t gen_valid[6],gen_pos[6],gen_fresh[6]; /* loop register: offsets, written slots, slot of the last hit, 1 if it was drawn anew */
 uint32_t gen_gate[6]; /* Chance decision stored per slot */
 uint32_t form_section,form_cycle,form_drop_mask; float form_pos;
 float gamaka_now[6]; int32_t gamaka_style[6]; float gamaka_age[6]; /* ornament offset in semitones now, its style, ms since the note */
 uint32_t midi_out_track[6]; /* note-ons queued per track (TRACKS and ZONES) */
 int32_t hits_now[6],rotate_now[6]; /* the hits and rotation the sequencer last used per track (Density, FORM, LFOs included) */
 /* Rhizome orbits as the engine holds them: position in steps since PLAY, each body's mean anomaly, periapsis direction
  * and eccentricity now (radians; direction 0 = right, -pi/2 = top), its last gate crossed (or refused by the
  * perturbation) at which step position, and the last conjunction (two bodies aligned with the star). moon_t0: the step
  * position of the note that spun a body's moons (-100: none yet), moon_note: what each moon plays after it,
  * moon_hit: the last moon that played (1..3). */
 float orbit_pos,orbit_M[6],orbit_w[6],orbit_e[6],orbit_hit_pos[6];int32_t orbit_hit_gate[6],orbit_hit_skip[6];uint32_t orbit_hits[6];
 int32_t conj_a,conj_b;float conj_pos;uint32_t conj_count;
 float moon_t0[6],moon_note[6][3];int32_t moon_hit[6];uint32_t moon_hits[6];
 /* The comet: its mean anomaly (0 = at the star), how strongly its passage acts now (0..1), passages so far. */
 float prof[6]; /* % of the processor over the last second of audio: sequencer and orbits, voice engines, voice chain, grains, effects and mix, all */
 float comet_M,comet_now,comet_filter_hz;uint32_t comet_passes,shocks; int32_t shock_left[6],shock_kind[6]; float shock_burst; /* shock_burst: a shock's jolt on the effects, 0..1 */ /* WANDER: shocks so far; each body's notes still shocked and how (1 skip, 2 push, 3 pitch) */ /* comet_filter_hz: the FILTER event's cutoff now, 0 when off */
 int32_t motif_at[6]; /* MOTIF: the phrase step each body looks from for its next note */ } md_stats;
/* Monades granular view: tape overview (256 columns, oldest wraps at the write head), grains as samples behind the head. */
enum { MD_TAPE_GRAINS=8 }; /* Rhizome: Beads' grains (behind: samples behind the write head, which moves toward lower indices; len: samples left) */
typedef struct { uint8_t peak[256]; uint32_t head,valid,length; int32_t freeze; float zone,zone_len; struct { float behind,len,env; int32_t active; } grain[MD_TAPE_GRAINS]; } md_tape;
void md_tape_view(md_tape *view);
/* Monades MIDI out. Generated notes (sequencer and repeats, never notes received over MIDI) go out by track
 * (mode 1: each track on its out channel, md_set_midi_channels) or by note zones (mode 2: every zone holding the note, channel 1-16, 0 = zone off;
 * overlapping zones send the note on each channel). clock: 24 ppqn with start / stop while the internal clock runs. */
typedef struct { int32_t mode,clock; struct { int32_t lo,hi,ch; } zone[8]; } md_midi_routing;
void md_set_midi_routing(const md_midi_routing *routing); /* audio owner (under plat_lock) */
/* Monades MIDI channels per track, 1-16, 0 = off (default: track n on channel n). in: the channel whose notes, bend
 * and CCs play the track (several tracks may share one). out: where TRACKS mode sends the track's notes. */
void md_set_midi_channels(const int32_t in[6],const int32_t out[6]); /* audio owner (under plat_lock) */
int md_midi_take(uint8_t msg[3]); /* next outgoing message (msg[0] = status), 1 if there was one */
unsigned md_scope(int track,float *dst); /* 128 points of the voice after its filter, oldest first */
void md_init(void);
int md_command(int command,int track,int value);
unsigned md_command_space(void);
void md_midi(unsigned status,unsigned a,unsigned b,int port); /* audio owner only */
int md_clock_source(void);
int md_midi_quantize(void);
void md_render(float *stereo,unsigned n);
const md_patch *md_state(void);
const md_stats *md_status(void);
int md_playing(void);
int md_validate(const md_patch *p);
int md_load(const md_patch *p); /* only while transport stopped, at the audio owner */
int md_decode(md_patch *out,const void *p,unsigned bytes); /* pure version migration + validation */
int md_import(const void *p,unsigned bytes); /* v1/v2/v3/v4 saved patches, stopped only */
unsigned md_memory_bytes(void);
int md_has_envelope(int model);
int md_cutoff_hz(int value);
int md_hit(unsigned step,unsigned hits,unsigned length,unsigned rotate);
int md_orbit_steps(int period); /* one turn, in sixteenths (a free period: at the tempo now, rounded) */
int md_orbit_tenths(int period); /* a free period's turn in tenths of a second, 0 for one that follows the tempo */
int md_orbit_moon_steps(int spin); /* one spin of the moons, in sixteenths */
const char *md_orbit_system_name(int system,int brief); /* JUPITER, KEPLER-223...; 0: OWN */
const char *md_orbit_system_about(int system); /* its bodies and ratios, a line */
int md_orbit_comet_steps(int comet); /* one turn of the comet, in sixteenths (0: none) */
enum { MD_COMET_ECC_MILLI=900 }; /* its eccentricity; its periapsis points down and to the left */
int md_orbit_gate_angles(const md_patch *p,int track,float *phi); /* directions of its gates (radians), returns the count */
float md_orbit_eccentric(float mean,float e); /* Kepler's equation: the eccentric anomaly E, M = E - e sin E */
float md_atan2(float y,float x);
const char *md_model_name(int model);
const char *md_knob_name(int model,int knob); /* Rhizome: what K1, K2, K3 do on this voice (named on SIX-OP and STRING MACHINE) */
const char *md_harm_name(int model,int harm); /* the chord or the DX7 sound K2 picks, or 0 when K2 is a plain amount */
unsigned md_voice_arena_spare(void); /* the least room SIX-OP or STRING MACHINE has left in a voice's arena so far (bytes) */
const char *md_biome_name(int id);
const char *md_biome_family(int id);
const char *md_tone_name(int id);
int md_tone_model(int id);
const char *md_scale_name(int scale);
int md_quantize(int note,int root,int scale);
unsigned md_scale_mask(int scale); /* ascent set; bit k = k semitones above the root */
unsigned md_scale_mask_down(int scale); /* descent set (differs for some ragas and classical melodic minor) */
const char *md_scale_family(int scale);
int md_scale_count(void);
int md_scale_custom(void); /* index of the CUSTOM scale */
int md_scale_at(int position); /* browsing order, grouped by family */
int md_scale_position(int scale);
void md_set_custom_scale(unsigned mask); /* audio owner; the root is always kept */
float md_quantize_pitch(float note,int root,int scale); /* fractional MIDI note for microtonal scales */
int md_scale_cents(int scale,float *degrees,int max,float *period); /* degrees in cents, root first; returns the count */
float md_lfo_wave(int track,int slot,uint32_t cycle,float phase); /* the LFO's shape (-1..1) at any cycle, random shapes included */
float md_lfo_sixteenths(int rate); /* cycle length in sixteenth notes for a synced rate, 0 for a free one */
float md_lfo_hz(int rate,int tempo);
const char *md_lfo_rate_name(int rate);
const char *md_lfo_shape_name(int shape);
const char *md_lfo_target_name(int target);
int md_send_hp_hz(int value); /* 0 = off */
float md_echo_ms(void); /* the echo time in use, synced or not */
/* Morph: from the patch now (A) towards a Monde (B, MD_MORPH_TO). MD_MORPH 0..1000 sets the position; numbers glide,
 * choices (engine, filter mode, scale, LFO shape...) switch half way. MD_MORPH_BARS 0 = by hand, 1..5 = PLAY carries it over
 * 4, 8, 16, 32 or 64 bars. MD_MORPH_SCOPE: what moves. MD_MORPH_KEEP keeps where it is and ends the morph; ENV goes back to A. */
enum { MD_MORPH_ALL, MD_MORPH_SOUND, MD_MORPH_RHYTHM, MD_MORPH_FX, MD_MORPH_SCOPES };
typedef struct { int32_t on,target,pos,bars,scope; } md_morph_state;
void md_morph_view(md_morph_state *s,const md_patch **a,const md_patch **b); /* a and b: the two Mondes (UI thread: read only) */
int md_morph_bars_len(int bars); /* 0, 4, 8, 16, 32, 64 */
int md_morph_point(int track); /* where a track switches (0..1000; 6 = scale and seed); it fades out and in around it */
unsigned md_morph_changes(void); /* bit per track: it has something to switch */
/* DERIVE: the Monde drifts on its own while PLAY runs, gliding towards variants drawn around its home (the patch when
 * the drift starts). MD_DRIFT_SLOW 0 = off, 1..6 = each glide lasts 8, 16, 32, 64, 128 or 256 bars. AMOUNT 0..2 near,
 * middle, far. VOYAGE 0 stay, 1 visit the family, 2 visit any Monde. SCOPE as the morph's. HOME: the patch now becomes
 * home. ENV goes back home. Events, newest first: 1 a track's number moved (value 0..4: cutoff, tone, harmonics,
 * shape, decay; up), 2 a track rests, 3 it comes back, 4 new notes, 5 towards a Monde (value), 6 home again. */
typedef struct { int32_t kind,track,value,up; } md_drift_event;
typedef struct { int32_t on,slow,amount,voyage,scope,pos,distance; float seconds,next; md_drift_event events[3]; } md_drift_state;
void md_drift_view(md_drift_state *s,const md_patch **home);
int md_drift_bars(int slow); /* 0, 8, 16, 32, 64, 128, 256 */
int md_can_return(void); /* 1 if ENV (RETURN) has a previous variation to go back to */
int md_unsaved(void); /* 1 if the patch changed since it was loaded or saved (seed, ECO drop, freeze left out) */
void md_mark_saved(void); /* the patch as it is now counts as saved (the app calls it under plat_lock) */
const char *md_echo_sync_name(int sync);
unsigned md_form_mask(unsigned cycle,unsigned section); /* tracks the form drops in that section */
float md_form_tension(unsigned section);
int md_form_bars(void);
const char *md_gen_source_name(int source);
const char *md_gamaka_name(int style);
float md_gamaka_shape(int style,int amount,float from,float ms); /* ornament offset (semitones) ms after a note; from = start offset */
float md_echo_sync_len16(int sync); /* the division in sixteenths, 0 = off */
#ifdef __cplusplus
}
#endif
