/* Shared pseudo-terminal harness for the editline test programs. */
#define _DEFAULT_SOURCE		/* forkpty(), cfmakeraw() under strict -std */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/wait.h>

#if defined(HAVE_PTY_H)
# include <pty.h>
#elif defined(HAVE_UTIL_H)
# include <util.h>
#elif defined(HAVE_LIBUTIL_H)
# include <libutil.h>
#endif

#include "editline.h"
#include "eltest.h"

#define MARK_BEG "<<<"
#define MARK_END ">>>"

/*
 * Start the pty in raw mode so the kernel line discipline does not eat
 * control keys (VKILL, VERASE, ...) before editline switches the terminal
 * itself, and pin the control chars editline reads back for determinism.
 */
static void setup_termios(struct termios *tio)
{
	memset(tio, 0, sizeof(*tio));
	cfmakeraw(tio);
	tio->c_cc[VINTR]  = 'C'  & 0x1f;
	tio->c_cc[VQUIT]  = '\\' & 0x1f;
	tio->c_cc[VERASE] = 0x7f;
	tio->c_cc[VKILL]  = 'U'  & 0x1f;
	tio->c_cc[VEOF]   = 'D'  & 0x1f;
	tio->c_cc[VSUSP]  = 'Z'  & 0x1f;
}

static pid_t spawn(int *master, const struct winsize *ws)
{
	struct termios tio;

	setup_termios(&tio);
	return forkpty(master, NULL, &tio, (struct winsize *)ws);
}

static int run_one(const struct testcase *tc)
{
	char buf[4096], result[1024] = "";
	char *beg, *end;
	ssize_t n;
	size_t len = 0;
	int master;
	pid_t pid;

	pid = spawn(&master, NULL);
	if (pid < 0)
		return -1;		/* SKIP: no pty */
	if (pid == 0) {
		char *line;

		el_no_echo = 1;
		if (tc->cf)
			rl_set_complete_func(tc->cf);
		line = readline("");
		printf(MARK_BEG "%s" MARK_END, line ? line : "(null)");
		fflush(stdout);
		_exit(0);
	}

	if (write(master, tc->input, strlen(tc->input)) < 0)
		perror("write");
	while (len < sizeof(buf) - 1) {
		n = read(master, buf + len, sizeof(buf) - 1 - len);
		if (n <= 0)
			break;		/* EOF or EIO when the child exits */
		len += (size_t)n;
		buf[len] = 0;
		if (strstr(buf, MARK_END))
			break;
	}
	waitpid(pid, NULL, 0);
	close(master);

	beg = strstr(buf, MARK_BEG);
	end = beg ? strstr(beg, MARK_END) : NULL;
	if (!beg || !end) {
		fprintf(stderr, "FAIL %-18s no result marker in output\n", tc->name);
		return 1;
	}
	beg += strlen(MARK_BEG);
	memcpy(result, beg, (size_t)(end - beg));
	result[end - beg] = 0;

	if (strcmp(result, tc->expect)) {
		fprintf(stderr, "FAIL %-18s expected [%s] got [%s]\n",
			tc->name, tc->expect, result);
		return 1;
	}

	printf("PASS %-18s [%s]\n", tc->name, result);
	return 0;
}

int el_run_cases(const char *suite, const struct testcase *cases, size_t count)
{
	size_t i;
	int fail = 0;

	for (i = 0; i < count; i++) {
		int rc = run_one(&cases[i]);

		if (rc < 0)
			return -1;	/* SKIP: no pty */
		fail += rc;
	}

	printf("\n%s: %zu tests, %d failures\n", suite, count, fail);
	return fail;
}

int el_capture(const char *input, int width, unsigned char *out, int outsz)
{
	struct winsize ws;
	ssize_t n;
	int master, len = 0;
	pid_t pid;

	memset(&ws, 0, sizeof(ws));
	ws.ws_col = (unsigned short)width;
	ws.ws_row = 24;

	pid = spawn(&master, &ws);
	if (pid < 0)
		return -1;		/* SKIP: no pty */
	if (pid == 0) {
		char *line = readline("");	/* echo on, no el_no_echo */

		(void)line;
		_exit(0);
	}

	if (write(master, input, strlen(input)) < 0)
		perror("write");
	while (len < outsz - 1) {
		n = read(master, out + len, outsz - 1 - len);
		if (n <= 0)
			break;
		len += (int)n;
	}
	waitpid(pid, NULL, 0);
	close(master);
	out[len] = 0;
	return len;
}
