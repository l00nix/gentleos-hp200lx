/* lib/bios.c */
extern void bios_putc(char c);
extern void bios_puts(const char *s);
extern uint16_t bios_getc(void);
/* lib/math.c */
extern int udiv32(uint32_t *out, uint32_t dividend, uint32_t divisor);
extern int umod32(uint32_t *out, uint32_t dividend, uint32_t divisor);
extern int add32(int32_t *out, int32_t a, int32_t b);
extern int sub32(int32_t *out, int32_t a, int32_t b);
/* lib/printf.c */
extern int vsnprintf(char *buf, size_t nbyte, const char *fmt, va_list va);
extern int snprintf(char *buf, size_t nbyte, const char *fmt, ...);
/* lib/rand.c */
extern void rand_init(void);
extern uint16_t rand(void);
/* lib/sleep.c */
extern void sleep(uint32_t msecs);
/* lib/string.c */
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *dest, int c, size_t n);
extern int32_t strcmp(const char *s1, const char *s2);
extern size_t strlen(const char *s1);
extern char *strncpy(char *dest, const char *src, size_t n);
/* lib/tests.c */
