#include "pdcpicocalc.h"

/* PicoCalc port: these were plain (non-static) globals, which happened to
   collide at link time with Empire's own unrelated global 'int cols;'
   (Empire's own idea of its display width, declared in extern.h) -- two
   different variables that never should have shared storage. They're
   only ever used inside this one file, so 'static' is both the fix and
   the more correct declaration. */
static uint16_t cols = 64;
static uint16_t rows = 32;

int PDC_get_columns(void)
{
    return cols;
}

int PDC_get_cursor_mode(void)
{
    return lcd_cursor_enabled() ? 1 : 0;
}

int PDC_get_rows(void)
{
    return rows;
}
