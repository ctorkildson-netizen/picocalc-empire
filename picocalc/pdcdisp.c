#include "pdcpicocalc.h"

void PDC_gotoyx(int y, int x)
{
    lcd_erase_cursor();
    lcd_move_cursor(x, y);
    lcd_draw_cursor();
}

void PDC_doupdate(void)
{

}

/* PicoCalc port: map a curses color number to the LCD's RGB565. Written
   as a switch on the COLOR_* macros (not a lookup table) because PDCurses
   numbers its colors differently depending on build options (PDC_RGB). */
static uint16_t _rgb565(short c)
{
    switch (c)
    {
    case COLOR_BLACK:   return 0x0000;
    case COLOR_RED:     return 0xF800;
    case COLOR_GREEN:   return 0x07E0;
    case COLOR_YELLOW:  return 0xFFE0;
    case COLOR_BLUE:    return 0x001F;
    case COLOR_MAGENTA: return 0xF81F;
    case COLOR_CYAN:    return 0x07FF;
    default:            return 0xFFFF; /* COLOR_WHITE and anything unknown */
    }
}

/* Set the LCD's drawing state from one cell's curses attributes. */
static void _set_attr(chtype attr)
{
    short fg = COLOR_WHITE, bg = COLOR_BLACK;
    short pair = PAIR_NUMBER(attr);

    if (pair && SP->color_started)
        pair_content(pair, &fg, &bg);

    lcd_set_reverse(false);
    lcd_set_foreground(_rgb565(fg));
    lcd_set_background(_rgb565(bg));
    lcd_set_reverse((attr & A_REVERSE) != 0);
    lcd_set_bold((attr & A_BOLD) != 0);
    lcd_set_underscore((attr & A_UNDERLINE) != 0);
}

/* Back to plain white-on-black, so the cursor (drawn in the current
   foreground colour) and anything drawn outside curses look normal. */
static void _reset_attr(void)
{
    lcd_set_reverse(false);
    lcd_set_foreground(0xFFFF);
    lcd_set_background(0x0000);
    lcd_set_bold(false);
    lcd_set_underscore(false);
}

/* PicoCalc port: previously this stripped every attribute (& 0x7F) and
   drew the whole span in one colour, so Empire's land/sea/unit colours
   never appeared. It now splits the span into runs of identical
   attributes and draws each run in its own colours. */
void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    char buffer[COLS + 1];

    if (len > COLS - x)
        len = COLS - x;

    int i = 0;
    while (i < len)
    {
        chtype attr = srcp[i] & (A_ATTRIBUTES & ~A_ALTCHARSET);
        int n = 0;

        while (i + n < len &&
               (srcp[i + n] & (A_ATTRIBUTES & ~A_ALTCHARSET)) == attr)
        {
            buffer[n] = srcp[i + n] & 0x7F;
            n++;
        }
        buffer[n] = '\0';

        _set_attr(attr);
        if (n == 1)
            lcd_putc(x + i, lineno, buffer[0]);
        else
            lcd_putstr(x + i, lineno, buffer);

        i += n;
    }

    _reset_attr();
}
