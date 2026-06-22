/* The Home and End keys arrive as multi-byte escape sequences, so reposition()
 * only ever sees the leading ESC and used to redraw them via a different path
 * than the single-byte Ctrl-A / Ctrl-E -- which scrolled a wrapped line.  Both
 * must now reposition the cursor identically; capture the editor's output for
 * each on a wrapping line and require it byte-for-byte equal. */
#include <config.h>
#include <stdio.h>
#include <string.h>

#include "editline.h"
#include "eltest.h"

int main(void)
{
	unsigned char ctl[8192], esc[8192];
	int nc, ne;

	/* 25 chars wrap on a 10-column terminal; jump home, then to the end. */
	nc = el_capture("0123456789abcdefghijklmno\001\005\r", 10, ctl, sizeof(ctl));
	ne = el_capture("0123456789abcdefghijklmno\033[H\033[F\r", 10, esc, sizeof(esc));
	if (nc < 0 || ne < 0)
		return 77;		/* SKIP: no pty */

	if (nc == ne && memcmp(ctl, esc, (size_t)nc) == 0) {
		printf("PASS homeend            Home/End reposition like Ctrl-A/Ctrl-E\n");
		printf("\nhomeend: 1 tests, 0 failures\n");
		return 0;
	}

	fprintf(stderr, "FAIL homeend            Home/End output differs from Ctrl-A/Ctrl-E\n");
	return 1;
}
