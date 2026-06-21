/* Phase 3: UTF-8 glyph-aware deletion.  Backspace and delete remove a
 * whole glyph; a single multibyte backspace must not enter the yank buffer.
 *
 * é = \303\251 (2 bytes). */
#include <config.h>

#include "editline.h"
#include "eltest.h"

static const struct testcase cases[] = {
	{ "utf8-backspace",    "a\303\251\010\r",          "a"            , NULL        },
	{ "utf8-del-fwd",      "\303\251ab\001\004\r",     "ab"           , NULL        },
	/* A single multibyte backspace must not land in the yank buffer:
	 * Ctrl-Y after it yanks nothing, so the result is "x" not "éx". */
	{ "utf8-bksp-no-yank", "\303\251\010\031x\r",      "x"            , NULL        },
};

int main(void)
{
	int rc = el_run_cases("utf8-delete", cases, sizeof(cases) / sizeof(cases[0]));

	if (rc < 0)
		return 77;		/* SKIP: no pty */
	return rc ? 1 : 0;
}
