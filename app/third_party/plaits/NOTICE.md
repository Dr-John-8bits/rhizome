# Plaits, in Rhizome

From Mutable Instruments' Plaits (1.2) by Emilie Gillet, MIT licence (the header of each file), as carried by Mutable
Dreams and Monades. Rhizome plays Monades' Plaits voices (FM, VA, additive, the drums) and two more of its engines:
SIX-OP FM and STRING MACHINE. The other engines are in the folder but not built. The sample rate is 44.1 kHz (`dsp.h`, as
in Mutable Dreams).

Changes for Rhizome, each marked `Rhizome:` in the code:
- `engine2/six_op_engine`: the six voices share one decoded bank of 96 DX7 patches (`set_patches`) instead of 5 KB of
  patches each; without a bank the engine renders silence;
- `engine/wavetable_engine.cc` (not built today): the wave map holds the four banks' 256 waves, not 64 (`LoadUserData` writes 256 pointers:
  the original allocation was overrun), and the smoothed coordinates are kept below 7 (they overshot to 7.0000019 and read
  wave 8, past the map);
- `fm/dx_units.h`: the exponent is shifted as an unsigned value (shifting a negative int is undefined in C++).

Rhizome also keeps the accent below 1 when it calls an engine: at exactly 1, the six-op's velocity curve reads one entry
past `lut_cube_root`.
