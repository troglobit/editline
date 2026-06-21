/* Shared pseudo-terminal test harness for the editline test programs.
 *
 * Each test program defines a table of cases (or drives el_capture
 * directly) and returns the failure count to the automake harness, or
 * exits 77 to mark the test SKIPPED when no pty is available.
 */
#ifndef ELTEST_H
#define ELTEST_H

#include <stddef.h>

typedef char *el_complete_fn(char *token, int *match);

struct testcase {
	const char     *name;	/* test label                          */
	const char     *input;	/* keystrokes to feed (NUL-terminated) */
	const char     *expect;	/* expected line returned by readline()*/
	el_complete_fn *cf;	/* optional custom completer           */
};

/*
 * Run each case: drive a real readline() over a pty with echo off, feed
 * the script, and compare the returned line.  Prints PASS/FAIL per case
 * and a one-line summary.  Returns the failure count, or -1 if no pty is
 * available (the caller should then exit 77 = SKIP).
 */
int el_run_cases(const char *suite, const struct testcase *cases, size_t count);

/*
 * Type `input` on a `width`-column terminal with echo on and capture the
 * editor's raw output into `out` (at most `outsz` bytes, NUL-terminated).
 * Returns the byte count, or -1 if no pty is available (caller exits 77).
 */
int el_capture(const char *input, int width, unsigned char *out, int outsz);

#endif /* ELTEST_H */
