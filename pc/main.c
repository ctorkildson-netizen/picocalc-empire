/* PC/SDL desktop entry point -- lets Empire run and be tested on a
   normal computer, without a PicoCalc, using PDCurses' own sdl2
   backend instead of the picocalc/ hardware backend. Mirrors the
   top-level (PicoCalc hardware) main.c: empire_main() no longer reads
   real command-line options (see patches/empire.patch), so argc/argv
   here are just placeholders to match its signature. */
#include <stdlib.h>

/* SDL2main.a's real Windows entry point (WinMain) calls a function
   named SDL_main -- it isn't looking for "main" at all. Including
   SDL.h is what makes that work: on platforms that need it, SDL's own
   header quietly '#define's main as SDL_main, so the ordinary main()
   below actually gets compiled under that name and the linker finds
   it. Leaving this include out compiles fine and fails only at the
   very last step, with an unrelated-looking "undefined reference to
   SDL_main". */
#include <SDL.h>

extern int empire_main(int argc, char *argv[]);

static char *empire_argv[] = { "empire" };

/* extern.h declares this; the PicoCalc build's own main.c supplies a
   hardware-reboot version. On a desktop, plain exit() is the real
   thing. */
void emp_exit(int status)
{
    exit(status);
}

/* SDL.h's rename only works if the signature matches what it declared
   for SDL_main exactly (int, argc/argv) -- a plain "int main(void)"
   compiles fine on its own but conflicts with that declaration once
   SDL.h is in scope. */
int main(int argc, char *argv[])
{
    /* Empire never returns from a normal game (empend() calls
       emp_exit()), but if it somehow does, shut down cleanly anyway. */
    empire_main(1, empire_argv);
    emp_exit(0);
    return 0;
}
