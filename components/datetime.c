/* See LICENSE file for copyright and license details. */
#include <time.h>
#include <stdio.h>

#include "../util.h"

void
datetime(char *out, const char *fmt)
{
	time_t t;

	t = time(NULL);
	if (!strftime(out, BUFF_SZ, fmt, localtime(&t))) {
		warn("strftime: Result string exceeds buffer size");
		ERRRET(out);
	}
}
