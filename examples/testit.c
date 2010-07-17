/*  $Revision: 5 $
**
**  A "micro-shell" to test editline library.
**  If given any arguments, commands aren't executed.
*/
#include <config.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "editline.h"

#ifndef HAVE_PERROR
void perror(char *s)
{
    extern int errno;

    (void)fprintf(stderr, "%s: error %d\n", s, errno);
}
#endif /* !HAVE_PERROR */

int main(int argc, char *argv[] __attribute__ ((unused)))
{
    char	*prompt;
    char	*p;
    int		doit;

    doit = argc == 1;
    if ((prompt = getenv("TESTPROMPT")) == NULL)
        prompt = "testit>  ";

    while ((p = readline(prompt)) != NULL) {
	(void)printf("\t\t\t|%s|\n", p);
	if (doit) {
	    if (strncmp(p, "cd ", 3) == 0) {
		if (chdir(&p[3]) < 0) {
		    perror(&p[3]);
                }
	    }
	    else if (system(p) != 0) {
		perror(p);
            }
        }
	add_history(p);
	free(p);
    }

    return 0;
}

/*
 * $PchId: testit.c,v 1.3 1996/02/22 21:18:51 philip Exp $
 */
