/* History file round-trip: read_history()/write_history() must preserve
 * lines of any length byte-for-byte.  A fixed read buffer used to split
 * long lines (and chop a byte mid-glyph for multibyte input). */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "editline.h"

#define IN  "history-in.tmp"
#define OUT "history-out.tmp"

static int files_equal(const char *a, const char *b)
{
	FILE *fa = fopen(a, "rb");
	FILE *fb = fopen(b, "rb");
	int ca, cb, eq = 1;

	if (!fa || !fb) {
		if (fa) fclose(fa);
		if (fb) fclose(fb);
		return 0;
	}
	do {
		ca = getc(fa);
		cb = getc(fb);
		if (ca != cb) {
			eq = 0;
			break;
		}
	} while (ca != EOF);
	fclose(fa);
	fclose(fb);

	return eq;
}

int main(void)
{
	FILE *fp;
	int i, fail = 0;

	/* A short ASCII line, a 300-byte line (150x "é") that exceeds the old
	 * 256-byte read buffer and would split mid-glyph, and another line. */
	fp = fopen(IN, "w");
	if (!fp) {
		perror(IN);
		return 77;		/* SKIP: cannot create scratch file */
	}
	fputs("short ascii line\n", fp);
	for (i = 0; i < 150; i++)
		fputs("\303\251", fp);
	fputc('\n', fp);
	fputs("trailing line\n", fp);
	fclose(fp);

	read_history(IN);
	write_history(OUT);

	if (!files_equal(IN, OUT)) {
		fprintf(stderr, "FAIL history-roundtrip  history file corrupted on round-trip\n");
		fail++;
	} else {
		printf("PASS history-roundtrip  [byte-for-byte]\n");
	}

	unlink(IN);
	unlink(OUT);
	printf("\nhistory: 1 tests, %d failures\n", fail);
	return fail ? 1 : 0;
}
