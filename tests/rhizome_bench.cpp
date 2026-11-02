// Rhizome bench: how much of the FM-1's audio time would each building block take?
//
// Every block renders the same number of samples as the FM-1 (44.1 kHz, quanta of 64) on the Mac, and its time is
// compared with two Monades Mondes whose load was read on an FM-1 (SYSTEM page, ECO, 2 November 2026):
// 87 CLOUDEE 69 % and 89 ALCHIMISTE 76 %. The estimate is a ratio of Mac times, so it is only as good as the
// assumption that the Mac and the FM-1 slow down the same way on every kind of code: confirm on the device.
// Built by tools/bench_rhizome.py (auto-vectorisation off, as the FM-1 has no SIMD unit).
// Built against app/compat like the FM-1's DSP (its fast maths, no C++ library): C headers only.
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dsp/dreams.h"
#include "stmlib/utils/buffer_allocator.h"
#include "plaits/resources.h"
#include "plaits/dsp/engine/additive_engine.h"
#include "plaits/dsp/engine/chord_engine.h"
#include "plaits/dsp/engine/fm_engine.h"
#include "plaits/dsp/engine/grain_engine.h"
#include "plaits/dsp/engine/modal_engine.h"
#include "plaits/dsp/engine/noise_engine.h"
#include "plaits/dsp/engine/particle_engine.h"
#include "plaits/dsp/engine/string_engine.h"
#include "plaits/dsp/engine/swarm_engine.h"
#include "plaits/dsp/engine/virtual_analog_engine.h"
#include "plaits/dsp/engine/waveshaping_engine.h"
#include "plaits/dsp/engine/wavetable_engine.h"
#include "plaits/dsp/engine2/chiptune_engine.h"
#include "plaits/dsp/engine2/phase_distortion_engine.h"
#include "plaits/dsp/engine2/six_op_engine.h"
#include "plaits/dsp/engine2/string_machine_engine.h"
#include "plaits/dsp/engine2/virtual_analog_vcf_engine.h"
#include "plaits/dsp/engine2/wave_terrain_engine.h"
#include "rings/dsp/part.h"
#include "rings/dsp/fx/reverb.h"
#ifdef RHIZOME_BEADS
#include "beads/dsp/processor.h"
#endif

static const int SR = 44100, Q = 64;
static double seconds = 10;
static int runs = 3;
static float L[Q], R[Q], A[Q];

// the FM-1's string.h has no search: two small helpers
static bool contains(const char *s, const char *w) { for (; *s; s++) { int i = 0; while (w[i] && s[i] == w[i]) i++; if (!w[i]) return true; } return false; }
static bool same(const char *a, const char *b) { while (*a && *a == *b) a++, b++; return *a == *b; }

static double now_ns() { timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1e9 + t.tv_nsec; }

// best of `runs`: nanoseconds of Mac time per second of audio
template <class S, class F> static double measure(S setup, F quantum) {
  double best = 1e30;
  const unsigned n = unsigned(seconds * SR / Q);
  for (int r = 0; r < runs; r++) {
    setup();
    for (unsigned q = 0; q < SR / Q / 4; q++) quantum(q);   // a quarter second to settle
    const double t0 = now_ns();
    for (unsigned q = 0; q < n; q++) quantum(q);
    best = std::min(best, (now_ns() - t0) / seconds);
  }
  return best;
}

struct Row { const char *group; char name[96]; double ns; };
static Row rows[64];
static int nrows = 0;
static void add(const char *group, const char *name, double ns) {
  Row &r = rows[nrows++];
  r.group = group; snprintf(r.name, sizeof r.name, "%s", name); r.ns = ns;
  fprintf(stderr, "  %-44s %10.0f ns/s\n", name, ns);
}

/* ---------- Monades, the yardstick ---------- */
static int monde_id(const char *name) {
  for (int i = 0; i < MD_BIOMES; i++) if (contains(md_biome_name(i), name)) return i;
  fprintf(stderr, "no Monde %s\n", name); exit(1);
}
static double monades(const char *name) {
  static float st[2 * Q];
  const int id = monde_id(name);
  return measure([&] { md_init(); md_command(MD_SCENE, 0, id); md_command(MD_QUALITY, 0, 0); md_command(MD_PLAY, 0, 1); },
                 [&](unsigned) { md_render(st, Q); });
}

/* ---------- Plaits engines, one voice each ---------- */
static char plaits_ram[65536];
template <class E> static double run_plaits(const uint8_t *user_data = nullptr) {
  static E *engine = nullptr;
  return measure([&] {
      delete engine; engine = new E();
      stmlib::BufferAllocator alloc(plaits_ram, sizeof plaits_ram);
      engine->Init(&alloc); engine->Reset();
      engine->LoadUserData(user_data);                     // as Plaits' voice does, NULL = the built-in waves
    }, [&](unsigned q) {
      plaits::EngineParameters p;
      p.trigger = (q % 345) == 0 ? plaits::TRIGGER_RISING_EDGE : plaits::TRIGGER_LOW;   // a note every 0.5 s
      p.note = 48.0f + float((q / 345) % 5) * 3.0f; p.timbre = 0.5f; p.morph = 0.45f; p.harmonics = 0.55f; p.accent = 0.8f;
      bool env = false;
      engine->Render(p, L, A, Q, &env);
    });
}

/* ---------- Rings: one part, polyphony 1 to 4 ---------- */
static uint16_t rings_reverb[32768];
static double run_rings(rings::ResonatorModel model, int poly) {
  static rings::Part *part = nullptr;
  return measure([&] {
      delete part; part = new rings::Part(); part->Init(rings_reverb); part->set_polyphony(poly); part->set_model(model);
    }, [&](unsigned q) {
      rings::PerformanceState ps = {};
      ps.internal_exciter = true; ps.internal_strum = false; ps.internal_note = false;
      ps.strum = (q % 172) == 0;                                               // a note every 0.25 s
      ps.tonic = 12.0f; ps.note = 36.0f + float((q / 172) % 4) * 5.0f; ps.fm = 0.0f; ps.chord = 0;
      rings::Patch patch = {0.4f, 0.6f, 0.65f, 0.3f};
      static float in[rings::kMaxBlockSize];
      for (int o = 0; o < Q; o += rings::kMaxBlockSize) {
        const int n = std::min<int>(rings::kMaxBlockSize, Q - o);
        std::fill(in, in + n, 0.0f);
        part->Process(ps, patch, in, L + o, A + o, n);
        ps.strum = false;
      }
    });
}

/* ---------- HALL: the Griesinger / Dattorro reverb of Rings and Beads (32768 x 16 bit) ---------- */
static uint16_t hall_ram[32768];
static double hall(int every) {                         // every = 1: 44.1 kHz; 2: the "vintage" 22 kHz
  static rings::Reverb *rv = nullptr;
  return measure([&] {
      delete rv; rv = new rings::Reverb(); rv->Init(hall_ram);
      rv->set_amount(0.5f); rv->set_input_gain(0.2f); rv->set_time(0.85f); rv->set_diffusion(0.7f); rv->set_lp(0.7f);
    }, [&](unsigned q) {
      const int n = Q / every;
      for (int i = 0; i < n; i++) L[i] = R[i] = ((q * 7 + i) % 97) * 0.002f - 0.1f;
      rv->Process(L, R, n);
    });
}

/* ---------- the tape delay, a first prototype ---------- */
// 22.05 kHz, 8-bit mu-law, 64 KiB of tape (about 3 s), three playback heads, speed with inertia, wow, flutter,
// drift, saturation at the record head, darker and noisier at each pass. Hermite interpolation at each head.
struct Tape {
  enum { N = 65536, MASK = N - 1 };
  uint8_t tape[N];
  int16_t decode[256];
  unsigned w = 0;
  float speed = 1, target = 1, wow = 0, flutter = 0, drift = 0, lp = 0, hp = 0, fb = 0.55f, prev = 0, held = 0;
  uint32_t rng = 1;
  float head[3] = {4000, 8000, 12000};
  static uint8_t encode(int s) {
    const int sign = s < 0 ? 0x80 : 0; if (s < 0) s = -s; if (s > 32635) s = 32635; s += 132;
    int e = 7; for (int m = 0x4000; (s & m) == 0 && e > 0; e--, m >>= 1) {}
    return uint8_t(~(sign | (e << 4) | ((s >> (e + 3)) & 0x0F)));
  }
  void init() {
    memset(tape, encode(0), sizeof tape);
    for (int u = 0; u < 256; u++) { int x = ~u & 0xFF, t = ((x & 0x0F) << 3) + 132; t <<= (x & 0x70) >> 4; decode[u] = int16_t((x & 0x80) ? 132 - t : t - 132); }
  }
  float at(float pos) const {
    const int i = int(pos); const float f = pos - float(i);
    const float xm = decode[tape[(i - 1) & MASK]], x0 = decode[tape[i & MASK]], x1 = decode[tape[(i + 1) & MASK]], x2 = decode[tape[(i + 2) & MASK]];
    const float c = (x1 - xm) * 0.5f, v = x0 - x1, wv = c + v, a = wv + v + (x2 - x0) * 0.5f, b = wv + a;
    return ((((a * f) - b) * f + c) * f + x0) * (1.0f / 32768.0f);
  }
  float noise() { rng = rng * 1664525u + 1013904223u; return float(int32_t(rng)) * (1.0f / 2147483648.0f); }
  // one sample at 22.05 kHz
  float step(float in, float t) {
    speed += 0.0005f * (target - speed);
    drift += 0.0002f * (noise() - drift);
    const float s = speed * (1 + 0.004f * sinf(6.2831853f * 0.7f * t) + 0.0015f * sinf(6.2831853f * 7.3f * t) + 0.01f * drift);
    float out = 0;
    for (int h = 0; h < 3; h++) out += at(float(w) - head[h] * s);
    out *= 0.4f;
    lp += 0.35f * (out - lp); hp += 0.004f * (lp - hp);
    float x = in + fb * (lp - hp) + 0.0005f * noise();
    x = x * (27 + x * x) / (27 + 9 * x * x);                               // soft saturation of the record head
    tape[w & MASK] = encode(int(std::max(-1.0f, std::min(1.0f, x)) * 32767));
    w++;
    return out;
  }
};
static Tape tape_unit;
static double tape() {
  return measure([&] { tape_unit = Tape(); tape_unit.init(); }, [&](unsigned q) {
      for (int i = 0; i < Q; i += 2) {                                     // write at 22.05 kHz, hold the output between
        const float t = float(q * Q + i) / SR, in = 0.5f * (L[i] + L[i + 1]);
        const float y = tape_unit.step(in, t);
        L[i] = 0.5f * (tape_unit.held + y); L[i + 1] = y; tape_unit.held = y;
      }
    });
}

/* ---------- the orbital sequencer: four bodies, Kepler's equation once a quantum ---------- */
static double orbits() {
  static float M[4], e[4] = {0.15f, 0.45f, 0.3f, 0.6f};
  return measure([&] { for (float &m : M) m = 0; }, [&](unsigned) {
      for (int b = 0; b < 4; b++) {
        M[b] += 0.0021f * float(b + 1); float E = M[b];
        for (int k = 0; k < 4; k++) E -= (E - e[b] * sinf(E) - M[b]) / (1 - e[b] * cosf(E));
        L[b] = cosf(E) - e[b]; R[b] = sqrtf(1 - e[b] * e[b]) * sinf(E);   // the body's x, y on its ellipse
      }
    });
}

#ifdef RHIZOME_BEADS
/* ---------- Beads, mono, a saw at its input ---------- */
static uint8_t beads_a[512 * 1024], beads_b[256 * 1024];
static uint16_t beads_reverb[32768];
static double beads_run(beads::Processor::Media media, float density) {
  static beads::Processor *proc = nullptr;
  static float phase = 0;
  return measure([&] {
      delete proc; proc = new beads::Processor();
      uint8_t *buffers[3] = {beads_a, beads_a + 256 * 1024, beads_b};
      proc->Init(buffers, beads_reverb); proc->Configure(media, true);
    }, [&](unsigned) {
      beads::Parameters p;
      p.time.base = 0.4f; p.time.attenuverter = 0; p.time.has_cv = false;
      p.size.base = 0.6f; p.size.attenuverter = 0; p.size.has_cv = false;
      p.shape.base = 0.4f; p.shape.attenuverter = 0; p.shape.has_cv = false;
      p.pitch.base = 0; p.pitch.attenuverter = 0; p.pitch.has_cv = false;
      p.monophonic_mode = false;
      beads::Processor::MixParameters m; m.feedback = 0.2f; m.dry_wet = 0.8f; m.reverb_amount = 0; m.output_gate = false;
      beads::ModulatedParameter d; d.base = density; d.cv = 0;
      static beads::FloatFrame in[32], out[32];
      static bool freeze[32];
      for (int o = 0; o < Q; o += 32) {
        for (int i = 0; i < 32; i++) { phase += 130.8f / SR; if (phase >= 1) phase -= 1; in[i].l = in[i].r = phase - 0.5f; }
        proc->Process(beads::SEED_BUTTON_STATE_OFF, nullptr, freeze, d, p, m, in, out, 32);
      }
    });
}
#endif

int main(int argc, char **argv) {
  const bool only_beads = argc > 1 && same(argv[1], "beads");
  if (getenv("BENCH_SECONDS")) seconds = atof(getenv("BENCH_SECONDS"));
  if (getenv("BENCH_RUNS")) runs = atoi(getenv("BENCH_RUNS"));
  if (!only_beads) {
    fprintf(stderr, "Monades (yardstick)\n");
    add("ref", "Monades 87 CLOUDEE (FM-1: 69 %)", monades("CLOUDEE"));
    add("ref", "Monades 89 ALCHIMISTE (FM-1: 76 %)", monades("ALCHIMISTE"));
    fprintf(stderr, "Plaits, one voice\n");
    add("plaits", "VA (in Monades)", run_plaits<plaits::VirtualAnalogEngine>());
    add("plaits", "FM 2-op (in Monades)", run_plaits<plaits::FMEngine>());
    add("plaits", "ADDITIVE (in Monades)", run_plaits<plaits::AdditiveEngine>());
    add("plaits", "WAVESHAPING", run_plaits<plaits::WaveshapingEngine>());
    add("plaits", "GRANULAR FORMANT", run_plaits<plaits::GrainEngine>());
    add("plaits", "WAVETABLE", run_plaits<plaits::WavetableEngine>());
    add("plaits", "CHORDS", run_plaits<plaits::ChordEngine>());
    add("plaits", "SWARM", run_plaits<plaits::SwarmEngine>());
    add("plaits", "NOISE", run_plaits<plaits::NoiseEngine>());
    add("plaits", "PARTICLE", run_plaits<plaits::ParticleEngine>());
    add("plaits", "STRING (inharmonic)", run_plaits<plaits::StringEngine>());
    add("plaits", "MODAL", run_plaits<plaits::ModalEngine>());
    add("plaits", "VA-VCF (1.2)", run_plaits<plaits::VirtualAnalogVCFEngine>());
    add("plaits", "PHASE DISTORTION (1.2)", run_plaits<plaits::PhaseDistortionEngine>());
    add("plaits", "SIX-OP FM, DX7 (1.2)", run_plaits<plaits::SixOpEngine>(plaits::syx_bank_0));
    add("plaits", "WAVE TERRAIN (1.2)", run_plaits<plaits::WaveTerrainEngine>());
    add("plaits", "STRING MACHINE (1.2)", run_plaits<plaits::StringMachineEngine>());
    add("plaits", "CHIPTUNE (1.2)", run_plaits<plaits::ChiptuneEngine>());
    fprintf(stderr, "Rings\n");
    add("rings", "RINGS MODAL x1", run_rings(rings::RESONATOR_MODEL_MODAL, 1));
    add("rings", "RINGS MODAL x4", run_rings(rings::RESONATOR_MODEL_MODAL, 4));
    add("rings", "RINGS STRING x1", run_rings(rings::RESONATOR_MODEL_STRING, 1));
    add("rings", "RINGS STRING x4", run_rings(rings::RESONATOR_MODEL_STRING, 4));
    add("rings", "RINGS SYMPATHETIC x2", run_rings(rings::RESONATOR_MODEL_SYMPATHETIC_STRING, 2));
    fprintf(stderr, "Effects and sequencer\n");
    add("fx", "HALL at 44.1 kHz (stereo)", hall(1));
    add("fx", "HALL vintage at 22 kHz (stereo)", hall(2));
    add("fx", "TAPE DELAY prototype (22 kHz, 3 heads)", tape());
    add("fx", "ORBITS, 4 bodies", orbits());
  }
#ifdef RHIZOME_BEADS
  const char *g = getenv("BEADS_LABEL") ? getenv("BEADS_LABEL") : "";
  fprintf(stderr, "Beads %s\n", g);
  char name[96];
  const struct { beads::Processor::Media m; const char *n; } media[] = {
    {beads::Processor::MEDIA_DIGITAL, "DIGITAL 16 bit"}, {beads::Processor::MEDIA_VINTAGE_DIGITAL, "VINTAGE 32k 12 bit"},
    {beads::Processor::MEDIA_CLEAN_TAPE, "CLEAN TAPE 24k 12 bit"}, {beads::Processor::MEDIA_DIRTY_TAPE, "DIRTY TAPE 24k 8 bit"}};
  for (auto &x : media) for (float d : (const float[]){0.65f, 0.9f}) {
    snprintf(name, sizeof name, "BEADS%s %s, density %.2f", g, x.n, d);
    add("beads", name, beads_run(x.m, d));
  }
#endif
  // CSV on stdout for the runner
  for (int i = 0; i < nrows; i++) printf("%s\t%s\t%.0f\n", rows[i].group, rows[i].name, rows[i].ns);
  return 0;
}
