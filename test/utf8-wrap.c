/* Phase 5: wrapped-line column accounting.  Type a run of multibyte
 * glyphs on a terminal narrow enough that the line wraps, with echo on,
 * and assert the editor never splits a glyph at the right margin: every
 * UTF-8 start byte must be followed by its full complement of
 * continuation bytes.  The old byte-based wrap math split them. */
#include <config.h>
#include <stdio.h>

#include "editline.h"
#include "eltest.h"

int main(void)
{
	/* 14x "€" (\342\202\254, 3 bytes) on a 10-column terminal.  The line
	 * wraps, and a byte-counted margin falls mid-glyph. */
	const char *in = "\342\202\254\342\202\254\342\202\254\342\202\254\342\202\254"
			 "\342\202\254\342\202\254\342\202\254\342\202\254\342\202\254"
			 "\342\202\254\342\202\254\342\202\254\342\202\254\r";
	unsigned char buf[8192];
	int len, split = 0;
	size_t k;

	len = el_capture(in, 10, buf, sizeof(buf));
	if (len < 0)
		return 77;		/* SKIP: no pty */

	/* Each UTF-8 start byte must be followed by its continuation bytes
	 * (0x80-0xBF); anything else means a glyph was split. */
	for (k = 0; k < (size_t)len && !split; k++) {
		int extra = 0, e;

		if (buf[k] >= 0xC2 && buf[k] <= 0xDF)
			extra = 1;
		else if (buf[k] >= 0xE0 && buf[k] <= 0xEF)
			extra = 2;
		else if (buf[k] >= 0xF0 && buf[k] <= 0xF4)
			extra = 3;

		for (e = 1; e <= extra; e++) {
			if (k + (size_t)e >= (size_t)len
			    || (buf[k + e] & 0xC0) != 0x80) {
				split = 1;
				break;
			}
		}
	}

	if (split) {
		fprintf(stderr, "FAIL wrap-no-split      glyph split across the margin\n");
		return 1;
	}
	printf("PASS wrap-no-split      [no split]\n");
	return 0;
}
