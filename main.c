#include <string.h>

#include "hardware/watchdog.h"

#include <drivers/audio.h>
#include <drivers/fat32.h>
#include <drivers/font.h>
#include <drivers/lcd.h>
#include <drivers/southbridge.h>

#include <curses.h>

extern int empire_main(int argc, char *argv[]);

static char *empire_argv[] = { "empire" };

void picocalc_reboot(int status)
{
    watchdog_reboot(0, 0, 0); // Reboot the device
}

void emp_exit(int status)
{
    lcd_enable_cursor(false);
    lcd_clear_screen();
    picocalc_reboot(status);
}

int main(void)
{
    // Initialise PicoCalc hardware
    sb_init();
    audio_init();
    lcd_init();
    fat32_init();

    lcd_enable_cursor(false);
    lcd_set_font(&font_8x10);
    lcd_clear_screen();

    lcd_set_bold(true);
    lcd_putstr(1, 2, "Empire: Wargame of the Century");
    lcd_set_bold(false);
    lcd_putstr(1, 4, "PicoCalc port in progress -- Chris Torkildson, 2026");

    // Empire never returns from a normal game (empend() calls emp_exit()),
    // but if it somehow does, make sure we still shut down cleanly.
    empire_main(1, empire_argv);
    emp_exit(0);
}
