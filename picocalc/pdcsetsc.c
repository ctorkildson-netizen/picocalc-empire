#include "pdcpicocalc.h"

int PDC_curs_set(int visibility)
{
    int ret_vis;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;
    SP->visibility = visibility;
    lcd_erase_cursor();
    lcd_enable_cursor(visibility > 0);

    return ret_vis;
}

/* PicoCalc port: PDCurses' start_color() (color.c) always calls this --
   every platform backend must supply it. Blair's original Rogue port
   never linked color.c in (Rogue on this backend doesn't call
   start_color()), so this was never missed. Empire does call it, by
   accident of a name collision explained in main.c/empire.h -- Empire's
   own "#ifdef A_COLOR" was meant to be an app-level on/off switch, but
   curses.h #defines A_COLOR as a real attribute-bit constant, so the
   #ifdef is always true once curses.h is in scope. That's fine: it means
   Empire's real (and pretty nice) color-by-terrain-type display just
   works once this hook exists. The PicoCalc's LCD is a real color
   screen with no need to simulate blinking text, so, matching the
   sdl1/sdl2 backends' minimal implementations, we just track the flag. */
int PDC_set_blink(bool blinkon)
{
    if (!SP)
        return ERR;

    if (SP->color_started)
        COLORS = 8;

    if (blinkon)
        SP->termattrs |= A_BLINK;
    else
        SP->termattrs &= ~A_BLINK;

    return OK;
}
