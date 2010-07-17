/*  $Revision: 5 $
**
**  A "micro-shell" to test editline library.
**  If given any arguments, commands aren't executed.
*/
#include <config.h>
#include <stdio.h>
#if	defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if	defined(HAVE_STRING_H)
#include <string.h>
#endif
#if	defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

extern char	*readline();
extern void	add_history();

#if	!defined(HAVE_STDLIB_H)
extern int	free();
extern int	system();
extern void	exit();
extern char	*getenv();
#endif	/* !defined(HAVE_STDLIB) */
#if   !defined(HAVE_STRING_H)
extern int	strncmp();
#endif
#if   !defined(HAVE_UNISTD_H)
extern int	chdir();
#endif

#if	!defined(HAVE_PERROR)
void
perror(s)
    char	*s;
{
    extern int	errno;

    (voidf)printf(stderr, "%s: error %d\n", s, errno);
}
#endif	/* defined(NEED_PERROR) */


/* ARGSUSED1 */
int
main(ac, av)
    int		ac;
    char	*av[] __attribute__ ((unused));
{
    char	*prompt;
    char	*p;
    int		doit;

    doit = ac == 1;
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
