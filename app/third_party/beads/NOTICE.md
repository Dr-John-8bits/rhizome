# Beads, in Rhizome

From Mutable Instruments' Beads by Emilie Gillet, MIT licence (the header of each file), taken from
github.com/pichenettes/eurorack on 2 November 2026. Only the granular processor is kept (no synth, no hardware code).

Changes for the FM-1, each marked `Rhizome:` in the code:
- memory: two banks of 64 KiB (128 KiB in all) instead of three of 256 KiB;
- 6 grains instead of 32 (`delay_granular_player.h`): about half the computation of 8;
- no reverb (Rhizome has its HALL), so `Processor::Init` takes no reverb buffer;
- fixed-size arrays instead of variable-length ones (at most 32 frames a call to `Process`), kept off the stack (static, or a shared scratch for the half-rate media): the FM-1's audio interrupt has 7.9 KB of stack;
- no `abs()` (the FM-1's library has none);
- `resources.cc` keeps only `lut_sine`;
- `ENABLE_HARDWARE_24K` off: the FM-1's codec stays at 44.1 kHz;
- accessors for the screen: the line's length and where the live grains read.
