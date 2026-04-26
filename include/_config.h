/*
 * Use custom color theme, see kernel/vga.c for the list
 * Only works on EGA/VGA cards, but not real CGA
 */
#define VGA_THEME 0

/*
 * Run test suite on startup
 */
#define ENABLE_TESTS 0

/*
 * Send debug output to UART (COM1)
 */
#define DEBUG_TO_UART 0

/*
 * Default date & time to use if we detect clock is not set,
 * by checking if current year < 2000. This is useful for
 * hardware with dead RTC battery
 */
#define DEFAULT_YEAR 2026
#define DEFAULT_MONTH 4
#define DEFAULT_DAY 24
#define DEFAULT_HOUR 14
#define DEFAULT_MINUTE 30
