/* Phase 4: word motion and case.  Word boundaries fall on glyph
 * boundaries (every glyph byte is a word char), so word kill removes
 * whole multibyte characters; M-u/M-l/M-c fold ASCII only.
 *
 * Also covers the fd_kill_word off-by-one fix (Meta-d once deleted one
 * byte too few).  é = \303\251 (2 bytes). */
#include <config.h>

#include "editline.h"
#include "eltest.h"

static const struct testcase cases[] = {
	{ "fd-kill-word",      "foo bar\001\033d\r",        " bar"          , NULL },
	{ "utf8-bk-word",      "caf\303\251 b\033bX\r",     "caf\303\251 Xb", NULL },
	{ "utf8-fd-kill-word", "caf\303\251 bar\001\033d\r"," bar"          , NULL },
	{ "utf8-bk-kill-word", "caf\303\251\027\r",         ""              , NULL },
};

int main(void)
{
	int rc = el_run_cases("utf8-word", cases, sizeof(cases) / sizeof(cases[0]));

	if (rc < 0)
		return 77;		/* SKIP: no pty */
	return rc ? 1 : 0;
}
