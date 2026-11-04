#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Rhizome on an FM-1: the processor load of each factory Monde, read over USB (the diagnostics SysEx, no flash writes).

  measure_fm1.py [--seconds 40] [--worst] [--mondes 0-17] [--port NAME]

For each Monde: load it, PLAY, reset the measured peak, let it play, then read the load now and its peak (the SYSTEM
page's figures), the audio dropouts (xruns), the overload fallbacks to ECO, and the longest audio block against its
budget. --worst also opens SPACE fully with Beads on CLEAN TAPE (its heaviest). The rule: the peak never reaches 85 %.
Writes a JSON next to the printout if --json is given. Needs mido with python-rtmidi.
"""
import argparse, json, struct, sys, time
import mido

SCENE, PLAY, MACRO, BEADS_MEDIA, CLEAN_TAPE = 20, 16, 14, 149, 1   # app/dsp/dreams.h
NAMES = ['LAST LIGHT', 'AMBER HOUR', 'LONG SHADOWS', 'SLOW EMBERS', 'NIGHT SWALLOWS', 'BLACK ORCHARD', 'SLEEPLESS TIDE',
         'NO STARS', 'FIRST BIRD', 'PALE COMET', 'CHOKED DAWN', 'DAY BREAKS', 'COLD IN THE DARK', 'KEPLER NIGHT',
         'TRAPPIST DUST', 'RAGA OF ASH', 'SLOW TIDES', 'LOOPS OF ASH']


class Device:
    def __init__(self, port=None):
        names = mido.get_input_names()
        self.name = port or next((n for n in names if 'FM-1' in n), None)
        if not self.name:
            sys.exit(f'no FM-1 among the MIDI ports: {names}')
        self.i, self.o, self.seq = mido.open_input(self.name), mido.open_output(self.name), 0
        time.sleep(.35)  # CoreMIDI may drop the first packet right after opening

    def call(self, c, p=()):
        self.seq = (self.seq + 1) % 128
        head = [0x7d, 0x4d, 0x44, c, self.seq]
        self.o.send(mido.Message('sysex', data=head + list(p)))
        deadline = time.monotonic() + 3
        while time.monotonic() < deadline:
            for msg in self.i.iter_pending():
                d = list(msg.data) if msg.type == 'sysex' else []
                if d[:5] != head:
                    continue
                if d[5]:
                    raise RuntimeError(f'command {c}: status {d[5]}')
                raw, at = [], 6
                while at < len(d):                      # 7 bytes behind each mask of their high bits
                    mask = d[at]; at += 1
                    for bit in range(min(7, len(d) - at)):
                        raw.append(d[at] | ((mask >> bit & 1) << 7)); at += 1
                time.sleep(.015)
                return bytes(raw)
            time.sleep(.002)
        raise TimeoutError(f'no answer to command {c}')

    def command(self, c, t, v):
        z = v + 8192
        self.call(2, [c, t, z & 127, z >> 7]); time.sleep(.05)

    def status(self):
        w = struct.unpack('<%dI' % 47, self.call(1)[:188])
        return dict(cpu=w[2], peak=w[3], xruns=w[4], overloads=w[5], quality=w[9], playing=w[10],
                    block_us_max=w[37], block_us_budget=w[38], quality_reductions=w[40])


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--seconds', type=int, default=40); ap.add_argument('--worst', action='store_true')
    ap.add_argument('--mondes', default='0-17'); ap.add_argument('--port'); ap.add_argument('--json')
    a = ap.parse_args()
    lo, _, hi = a.mondes.partition('-'); ids = range(int(lo), int(hi or lo) + 1)
    dev = Device(a.port); results = []
    print(f'{dev.name}: {a.seconds} s a Monde' + (', SPACE 100 on CLEAN TAPE' if a.worst else ''), flush=True)
    for k in ids:
        if dev.status()['playing']:
            dev.command(PLAY, 0, 0); time.sleep(.3)
        dev.command(SCENE, 0, k)
        if a.worst:
            dev.command(MACRO, 3, 1000); dev.command(BEADS_MEDIA, 0, CLEAN_TAPE)
        dev.command(PLAY, 0, 1); time.sleep(2)
        dev.call(3); before = dev.status(); time.sleep(a.seconds); s = dev.status()
        s.update(monde=k + 1, name=NAMES[k] if k < len(NAMES) else str(k), xruns_during=s['xruns'] - before['xruns'],
                 overloads_during=s['overloads'] - before['overloads'])
        results.append(s)
        flag = '  <-- ABOVE 85 %' if s['peak'] >= 85 else ''
        print(f"{s['monde']:2} {s['name']:16} load {s['cpu']:3} %  peak {s['peak']:3} %  block max {s['block_us_max']:5} us"
              f" / {s['block_us_budget']} us  xruns +{s['xruns_during']}  ECO fallbacks +{s['overloads_during']}{flag}", flush=True)
    dev.command(PLAY, 0, 0)
    worst = max(results, key=lambda r: r['peak'])
    print(f"highest peak: {worst['peak']} % ({worst['name']}); ceiling 85 %")
    if a.json:
        open(a.json, 'w').write(json.dumps(results, indent=1) + '\n')


if __name__ == '__main__':
    main()
