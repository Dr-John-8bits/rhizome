#!/usr/bin/env python3
"""Rhizome bench: build tests/rhizome_bench.cpp on the Mac, run it, and estimate each block's load on an FM-1.

  python tools/bench_rhizome.py [--beads EURORACK_DIR] [--seconds 10] [--runs 3]

The estimate scales Mac times by a factor measured on 2 November 2026 with two Monades Mondes whose load was read on an
FM-1 (87 CLOUDEE 69 %, 89 ALCHIMISTE 76 %, ECO). Auto-vectorisation is off, as the FM-1 has no SIMD unit. A Mac estimate, not a
measurement: the device bench confirms it.
--beads: a checkout of github.com/pichenettes/eurorack (MIT); Beads is copied into the build folder and built twice,
with its 32 grains and with 8.
"""
import argparse, csv, io, os, re, shutil, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

R = Path(__file__).resolve().parents[1]
A = R / 'app'
T = A / 'third_party'
OUT = A / 'build' / 'bench'
# FM-1 % per Mac ns/s. Calibrated on 2 November 2026 with two Monades Mondes whose load was read on an FM-1 (87 CLOUDEE
# 69 %, 89 ALCHIMISTE 76 %, ECO): 3.514e-6 and 3.362e-6, their mean below. Rhizome no longer carries Monades' voices, so
# the yardstick cannot be re-run here: a measurement on the FM-1 will replace this figure.
SCALE = 3.438e-6 * 2.4
# x 2.4: on 4 November 2026 the first Mondes measured 121 to 130 % on the FM-1 where this bench said 47 to 53 %. Plaits'
# voices slow down about 1.5 times more than Monades' did on the device, and Beads about 4 times: a single factor is
# only a rough guide. The device's own reading (tools/measure_fm1.py) is the one that counts.

PLAITS = ['plaits/resources.cc', 'plaits/dsp/chords/chord_bank.cc', 'plaits/dsp/fm/algorithms.cc', 'plaits/dsp/fm/dx_units.cc',
          *[f'plaits/dsp/physical_modelling/{n}.cc' for n in ('modal_voice', 'resonator', 'string', 'string_voice')],
          *[f'plaits/dsp/engine/{n}_engine.cc' for n in ('additive', 'chord', 'fm', 'grain', 'modal', 'noise', 'particle',
                                                            'string', 'swarm', 'virtual_analog', 'waveshaping', 'wavetable')],
          *[f'plaits/dsp/engine2/{n}_engine.cc' for n in ('chiptune', 'phase_distortion', 'six_op', 'string_machine',
                                                             'virtual_analog_vcf', 'wave_terrain')]]
RINGS = ['rings/resources.cc', 'rings/dsp/part.cc', 'rings/dsp/string.cc', 'rings/dsp/resonator.cc', 'rings/dsp/fm_voice.cc']


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--beads', type=Path, help='checkout of pichenettes/eurorack')
    ap.add_argument('--seconds', type=float, default=10)
    ap.add_argument('--runs', type=int, default=3)
    a = ap.parse_args()

    conf = (A / 'fm1.toml').read_text()
    monades = re.findall(r'"([^"]+\.(?:cc|cpp))"', conf.split('o2 =', 1)[1].split('\n', 1)[0])
    sdk = subprocess.run(['xcrun', '--show-sdk-path'], capture_output=True, text=True).stdout.strip()
    B = R / 'base'
    inc = [A, B / 'host', B / 'device', B / 'include', B / 'lib', A / 'build' / 'gen', T, A / 'compat']
    # the device's own flags for the DSP (fm1.toml o2_cflags), its fast maths (compat/), no auto-vectorisation
    flags = ['-O3', '-ffp-contract=off', '-DPLAT_HOST', '-DTEST', '-fno-vectorize', '-fno-slp-vectorize', '-std=c++11',
             '-fno-exceptions', '-fno-rtti', '-w', *[f'-I{p}' for p in inc], *(['-isysroot', sdk] if sdk else [])]
    OUT.mkdir(parents=True, exist_ok=True)

    def compile_all(sources, tag, extra=()):
        def one(src):
            o = OUT / (tag + '_' + re.sub(r'[/.]', '_', str(src)) + '.o')
            subprocess.run(['clang++', *flags, *extra, '-c', str(src), '-o', str(o)], check=True)
            return str(o)
        with ThreadPoolExecutor(os.cpu_count() or 4) as ex:
            return list(ex.map(one, sources))

    base_sources = [A / s for s in monades] + [T / s for s in PLAITS + RINGS if (A / s) not in [A / m for m in monades]]
    base_sources = list(dict.fromkeys(base_sources))
    # Monades' own list already has some Plaits and Rings files: keep one copy of each
    seen, uniq = set(), []
    for s in base_sources:
        key = s.resolve()
        if key not in seen:
            seen.add(key); uniq.append(s)
    print(f'building {len(uniq)} files…', file=sys.stderr)
    objs = compile_all(uniq, 'base')

    builds = [('rhizome-bench', [], ())]
    if a.beads:
        src = a.beads / 'beads'
        if not (src / 'dsp' / 'processor.cc').exists():
            sys.exit(f'no beads/dsp/processor.cc under {a.beads}')
        for grains in (32, 8):
            root = OUT / f'beads{grains}'
            shutil.rmtree(root, ignore_errors=True)
            shutil.copytree(src / 'dsp', root / 'beads' / 'dsp')
            for f in ('resources.cc', 'resources.h'):
                shutil.copy(src / f, root / 'beads' / f)
            player = root / 'beads' / 'dsp' / 'delay_granular_player.h'
            player.write_text(player.read_text().replace('GranularPlayer<DelayGrain, 32>', f'GranularPlayer<DelayGrain, {grains}>'))
            wt = root / 'beads' / 'dsp' / 'wavetable_granular_player.h'
            wt.write_text(wt.read_text().replace('GranularPlayer<WavetableGrain, 32>', f'GranularPlayer<WavetableGrain, {grains}>'))
            bsrc = sorted((root / 'beads' / 'dsp').glob('*.cc')) + [root / 'beads' / 'resources.cc']
            extra = (f'-I{root}', '-DRHIZOME_BEADS')
            # Beads prints debug lines under TEST and calls abs() on integers: neither exists on the FM-1 yet
            beads_only = ('-include', 'stdlib.h', '-include', 'stdio.h')
            builds.append((f'rhizome-bench-beads{grains}', compile_all(bsrc, f'beads{grains}', extra + beads_only), extra))

    results = []
    for i, (name, extra_objs, extra) in enumerate(builds):
        exe = OUT / name
        subprocess.run(['clang++', *flags, *extra, str(R / 'tests' / 'rhizome_bench.cpp'), *objs, *extra_objs, '-o', str(exe)], check=True)
        env = dict(os.environ, BENCH_SECONDS=str(a.seconds), BENCH_RUNS=str(a.runs),
                   BEADS_LABEL='' if not extra else f' ({name[-2:].lstrip("s")} grains)')
        args = [] if i == 0 else ['beads']
        print(f'running {name}…', file=sys.stderr)
        out = subprocess.run([str(exe), *args], capture_output=False, stdout=subprocess.PIPE, text=True, env=env, check=True).stdout
        results += list(csv.reader(io.StringIO(out), delimiter='\t'))

    s = SCALE
    print('FM-1 estimate from the 2 November 2026 calibration (Mac speed varies: a few % either way)')
    print(f'{"block":44} {"Mac ns/s":>10} {"FM-1 est.":>10}')
    for group, name, v in results:
        print(f'{name:44} {float(v):10.0f} {float(v) * s:9.1f} %')


if __name__ == '__main__':
    main()
