# Empire on PicoCalc

A port of **Empire** — Chuck Simmons' 1987–88 strategy game, as maintained by
Walter Bright at
[DigitalMars/Empire-for-VMS](https://github.com/DigitalMars/Empire-for-VMS)
(version 1.2) — to the [PicoCalc](https://www.clockworkpi.com/product-page/picocalc)
handheld, targeting the RP2350. Built the same way
[Blair Leduc's picocalc-rogue](https://github.com/BlairLeduc/picocalc-rogue)
was: the original game's source sits untouched as vendored code, with a small
patch set applied on top, running on a PDCurses hardware backend rather than
a hand-rolled shim.

**This file is a PicoCalc-specific supplement, not a replacement for the
original manual.** Empire's own `README.md`, `COPYING` (the Empire General
Public License), and man page (`vms-empire.6`) ship untouched with the
vendored source and remain the authority on how the game itself is played.
This document only covers what's different about running it on a handheld
instead of a VAX.

A note on manuals: if you go looking for Empire documentation online, you
may land on Eric Raymond's actively-maintained `vms-empire`
(catb.org/~esr/vms-empire), currently at version 1.22. It's a real, ongoing
fork — and a good read — but it's a different tree from the one this port
targets (1.2), and it documents extra options (`-g` map geometry, `-t` torus
wraparound) that don't exist in this codebase. Don't be surprised if a
manual you find doesn't match what's running on the device.

## What's different on the PicoCalc

**No command-line options.** There's no `argv` and no `getopt()` on the
device. The original `-w` (water), `-s` (smooth), `-d` (delay), and `-S`
(save interval) flags have been removed entirely rather than faked — they're
hardcoded to the exact same values the original used when run with no flags
at all (`70`, `5`, `2000`, `10`). There is currently no way to change these
without editing and recompiling the port.

**Map size and unit count are fixed, and smaller than the original.** The
map is 64×40 (was 100×60) and the piece pool is capped at 300 units (was
5000). This isn't a difficulty setting — it's a hard limit of the RP2350's
520KB of onboard RAM, measured directly against a linked build rather than
estimated. There is no `-g`-style option to change it, on this device or in
the original codebase this port targets.

**Saving is fully automatic — you cannot lose more than a few turns of
progress, and you don't have to do anything.** This is inherited directly
from the original 1988 code, unmodified: the game autosaves to `empsave.dat`
every 10 turns and again on quit, and it automatically tries to restore that
file the instant it starts. Unlike Rogue's save-and-quit (single use, deleted
on restore, by design), Empire's save is ordinary and reusable — you can
resume the same save any number of times. There's no save/load menu and no
key to remember.

**If there's no SD card in the device at startup**, you'll see a message
offering to wait for one to be inserted, or to continue without saving.
Playing without a card works completely normally in every other way — the
game just silently never saves, since every attempt to open `empsave.dat`
fails safely (no crash, nothing drawn on top of the game screen) rather than
retrying or warning you again mid-session. If you skip the prompt, treat it
as your only warning.

## Credits

- **Empire**: Copyright (C) 1987, 1988 Chuck Simmons, under the Empire
  General Public License (see `COPYING` in the vendored source).
- **Maintained source**: Walter Bright,
  [DigitalMars/Empire-for-VMS](https://github.com/DigitalMars/Empire-for-VMS).
- **Architectural template**: Blair Leduc,
  [picocalc-rogue](https://github.com/BlairLeduc/picocalc-rogue) — the
  PDCurses-on-PicoCalc backend and patch-based build this port follows.
- **PicoCalc port**: Chris Torkildson, 2026.
