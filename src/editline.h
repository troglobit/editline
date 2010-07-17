/*  $Revision: 5 $
**
**  Internal header file for editline library.
*/
#ifndef __PRIVATE_EDITLINE_H__
#define __PRIVATE_EDITLINE_H__

#include <config.h>
#include <stdio.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef SYS_UNIX
#include "unix.h"
#endif
#ifdef SYS_OS9
#include "os9.h"
#endif

#ifndef SIZE_T
#define SIZE_T	unsigned int
#endif

typedef unsigned char	CHAR;

#define MEM_INC		64
#define SCREEN_INC	256

#define DISPOSE(p)		free((char *)(p))
#define NEW(T, c)		((T *)malloc((unsigned int)(sizeof (T) * (c))))
#define RENEW(p, T, c)	        (p = (T *)realloc((char *)(p), (unsigned int)(sizeof (T) * (c))))
#define COPYFROMTO(new, p, len)	(void)memcpy((char *)(new), (char *)(p), (int)(len))

/*
**  Variables and routines internal to this package.
*/
extern int	rl_eof;
extern int	rl_erase;
extern int	rl_intr;
extern int	rl_kill;
extern int	rl_quit;
#ifdef DO_SIGTSTP
extern int	rl_susp;
#endif
#ifdef COMPLETE
extern char	*default_rl_complete();
extern int	default_rl_list_possib(char *pathname, char ***avp);
#endif
extern void     rl_ttyset(int Reset);
extern void	rl_add_slash(char *path, char *p);

#ifndef HAVE_STDLIB_H
extern char	*getenv();
extern char	*malloc();
extern char	*realloc();
extern char	*memcpy();
extern char	*strcat();
extern char	*strchr();
extern char	*strrchr();
extern char	*strcpy();
extern char	*strdup();
extern int	strcmp();
extern int	strlen();
extern int	strncmp();
#endif/* !HAVE_STDLIB */

#ifdef NEED_STRDUP
extern char	*strdup(const char *s);
#endif

#include "../include/editline.h"
#endif  /* __PRIVATE_EDITLINE_H__ */
