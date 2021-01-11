#include <stdint.h>
#include <stdarg.h>

#define BUFF_SZ 96
/*
 * buffer size for one commponent
 * if you are using `wifi.c` it must be at least 78 bytes
 */

#define ONCE ((unsigned int) -1)  /* for config.h */
#define LEN(S) (sizeof(S) / sizeof *(S))
#define ERRRET(B) do { (B)[0] = '\0'; return; } while (0)

void warn(const char *, ...);
void bprintf(char *, const char *, ...);
int  pscanf(const char *, const char *, ...);
int  esnprintf(char *, size_t, const char *, ...);
void fmt_human(char *, uintmax_t, unsigned short int);
