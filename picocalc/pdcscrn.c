#include "pdcpicocalc.h"

void PDC_picocalc_start_keyboard(void);

void PDC_scr_close(void)
{
    PDC_LOG(("PDC_scr_close() - called\n"));
}

int PDC_scr_open(void)
{
    PDC_LOG(("PDC_scr_open() - called\n"));

    SP->orig_attr = FALSE;
    SP->audible = TRUE;
    SP->mono = FALSE;      /* PicoCalc port: the LCD is a colour screen (see pdcdisp.c) */
    SP->termattrs = A_UNDERLINE | A_REVERSE | A_BOLD | A_COLOR;
    SP->_preserve = FALSE;

    /* PicoCalc port: pick the LCD font to match the column count curses
       is about to use (PDC_get_columns(), hardcoded 64 in pdcgetsc.c).
       Previously the font was only switched in PDC_resize_screen(), which
       initscr() never calls -- so the LCD stayed in main.c's 8x10 banner
       font (40 columns) while curses drew 64-column lines, and any line
       longer than 40 characters overran lcd_putstr()'s one-row pixel
       buffer and came out garbled. Colossal Cave works around the same
       thing with an explicit resize_term() after initscr(); doing it here
       fixes it for any app on this backend. */
    lcd_set_font(PDC_get_columns() > 40 ? &font_5x10 : &font_8x10);
    lcd_clear_screen();

    /* Start keyboard polling regardless of whether the app ever calls
       raw() -- see PDC_picocalc_start_keyboard() in pdckbd.c. */
    PDC_picocalc_start_keyboard();

    /* PicoCalc port: curses starts with the cursor visible
       (SP->visibility = 1) but nothing ever told the LCD driver, and
       main.c turns the LCD cursor off for its banner -- so there was no
       cursor at all unless the app happened to call curs_set(). */
    lcd_enable_cursor(true);

    return OK;
}

int PDC_resize_screen(int nlines, int ncols)
{
    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
             nlines, ncols));

    if (ncols > 40)
    {
        lcd_set_font(&font_5x10);
    }
    else
    {
        lcd_set_font(&font_8x10);
    }

    return OK;
}

void PDC_reset_prog_mode(void)
{
    PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
}

void PDC_reset_shell_mode(void)
{
    PDC_LOG(("PDC_reset_shell_mode() - called.\n"));
}

void PDC_restore_screen_mode(int i)
{
}

void PDC_save_screen_mode(int i)
{
}

bool PDC_can_change_color(void)
{
    return FALSE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    return OK;
}