Following up on the thread where I tried (and failed) to talk Blair into porting Empire — I'm going to give it a go myself, targeting the PicoCalc on RP2350.

Before getting into that, I want to be specific about what I'm borrowing from `picocalc-rogue`, because it's more than inspiration. Blair's approach is genuinely the right way to do this kind of port, and I don't think that's an accident:

- Rogue's own source sits untouched, as a plain git submodule, with a small patch set applied on top rather than hand-edited in place. That means upstream fixes are still mergeable, and the actual scope of the port is visible in a couple of small diffs instead of buried across the whole codebase.
- Rather than hand-rolling a curses-compatible shim, he wrote a real PDCurses platform port — the same pattern PDCurses already uses for DOS, SDL, X11, and Windows console. That's the difference between "a shim that happens to work for Rogue" and "a first-class backend Rogue happens to be the first thing running on."
- The actual patches are tiny. Two lines in PDCurses (a display-refresh timing tweak), and the Rogue-side changes are just what a firmware entry point needs instead of a process entry point — no rewritten game logic anywhere.

Empire's a different animal in one specific way worth naming honestly: its original data structures assume a VAX with effectively unlimited RAM — a handful of full-map scratch buffers plus a pool sized for 5,000 simultaneous units add up to roughly 880KB, against the RP2350's 520KB of SRAM. So the actual work here isn't display plumbing (that part's solved, thanks to the above), it's re-budgeting forty-year-old assumptions about memory for a chip a thousand times smaller than what they were written for.

Which points at something bigger than either of our two projects: this combination — PDCurses plus a clean hardware starter kit underneath it — isn't really a Rogue thing or an Empire thing. It's a template for porting any curses-based Unix program to the PicoCalc, full stop. The display and input problem is generically solved once. The only question that changes program to program is whether that program's data actually fits in the chip's memory budget — and that's a countable, answerable question, not a fuzzy one. If there's an old curses program you've been carrying a torch for, the hard part's already done for you. The only thing left to find out is whether the numbers work.

More as this progresses.

—

Research and some of the archaeology here (license terms, memory-footprint numbers, PDCurses internals) done with Claude's help.
