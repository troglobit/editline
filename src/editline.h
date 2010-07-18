/* Internal header file for editline library.
 *
 * Copyright (c) 1992, 1993  Simmule Turner and Rich Salz. All rights reserved.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or of the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 * 4. This notice may not be removed or altered.
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
#ifdef CONFIG_SIGSTOP
extern int	rl_susp;
#endif
#ifdef CONFIG_DEFAULT_COMPLETE
extern char	*default_rl_complete(char *pathname, int *unique);
extern int	default_rl_list_possib(char *pathname, char ***avp);
#endif
extern void     rl_ttyset(int Reset);
extern void	rl_add_slash(char *path, char *p);

#ifndef HAVE_STDLIB_H
extern char	*getenv(const char *name);
extern char	*malloc(size_t size);
extern char	*realloc(void *ptr, size_t size);
extern char	*memcpy(void *dest, const void *src, size_t n);
extern char	*strcat(char *dest, const char *src);
extern char	*strchr(const char *s, int c);
extern char	*strrchr(const char *s, int c);
extern char	*strcpy(char *dest, const char *src);
extern char	*strdup(const char *s);
extern int	strcmp(const char *s1, const char *s2);
extern int	strlen(const char *s);
extern int	strncmp(const char *s1, const char *s2, size_t n);
#endif/* !HAVE_STDLIB */

#ifndef HAVE_STRDUP
extern char	*strdup(const char *s);
#endif

#include "../include/editline.h"
#endif  /* __PRIVATE_EDITLINE_H__ */
