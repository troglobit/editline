/* Phase 2: UTF-8 glyph-aware cursor movement.  Inserting and moving by
 * char must land on glyph boundaries, never mid-character.
 *
 * é = \303\251 (2 bytes), € = \342\202\254 (3 bytes). */
#include <config.h>

#include "editline.h"
#include "eltest.h"

static const struct testcase cases[] = {
	{ "utf8-insert",       "\303\251\342\202\254\r",   "\303\251\342\202\254", NULL },
	{ "utf8-back-glyph",   "a\303\251b\002\002X\r",    "aX\303\251b"  , NULL        },
	{ "utf8-fwd-glyph",    "\303\251ab\001\006X\r",    "\303\251Xab"  , NULL        },
#ifdef CONFIG_ANSI_ARROWS
	{ "utf8-arrow-left",   "\303\251\033[DX\r",        "X\303\251"    , NULL        },
#endif
};

int main(void)
{
	int rc = el_run_cases("utf8-move", cases, sizeof(cases) / sizeof(cases[0]));

	if (rc < 0)
		return 77;		/* SKIP: no pty */
	return rc ? 1 : 0;
}
