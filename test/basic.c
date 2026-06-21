/* Phase 1: the original ASCII editing harness -- insert, cursor motion,
 * kill, word motion, transpose, arrow keys and completion. */
#include <config.h>
#include <stdlib.h>
#include <string.h>

#include "editline.h"
#include "eltest.h"

/* Custom completers, exercising the no-shell-quote path (#5). */
static char *comp_plain(char *token, int *match)
{
	(void)token;
	*match = 1;
	return strdup("ello");
}

static char *comp_space(char *token, int *match)
{
	(void)token;
	*match = 1;
	return strdup("lo go");
}

static const struct testcase cases[] = {
	{ "insert",            "hello\r",                  "hello"        , NULL        },
	{ "backspace",         "hellx\010o\r",             "hello"        , NULL        },
	{ "back-char-insert",  "abc\002\002X\r",           "aXbc"         , NULL        },
	{ "beg-end-line",      "abc\001X\005Y\r",          "XabcY"        , NULL        },
	{ "kill-to-end",       "abcdef\001\006\006\013\r", "ab"           , NULL        },
	{ "kill-to-begin",     "abc\002\025\r",            "c"            , NULL        },
	{ "word-forward",      "foo bar\001\033fX\r",      "fooX bar"     , NULL        },
	{ "transpose",         "ab\024\r",                 "ba"           , NULL        },
#ifdef CONFIG_ANSI_ARROWS
	{ "arrow-left",        "abc\033[D\033[DX\r",       "aXbc"         , NULL        },
#endif
	{ "complete-plain",    "h\t\r",                    "hello"        , comp_plain  },
	{ "complete-space",    "h\t\r",                    "hlo go"       , comp_space  },
};

int main(void)
{
	int rc = el_run_cases("basic", cases, sizeof(cases) / sizeof(cases[0]));

	if (rc < 0)
		return 77;		/* SKIP: no pty */
	return rc ? 1 : 0;
}
