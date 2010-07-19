/* Minix editline
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
#ifndef __EDITLINE_H__
#define __EDITLINE_H__

/* Display print 8-bit chars as `M-x' or as the actual 8-bit char?  (Default:1) */
extern int rl_meta_chars;

/* Assign these to get command completion, see cli.c for example usage. */
extern char *(*rl_complete)   (char *token, int *match);
extern int   (*rl_list_possib)(char *token, char ***av);

/* For compatibility with FSF readline. */
extern int         rl_point;
extern int         rl_mark;
extern int         rl_end;
extern char       *rl_line_buffer;
extern const char *rl_readline_name;
extern void rl_reset_terminal(char *p);
extern void rl_initialize(void);

extern char *readline(const char *prompt);
extern void add_history(char *line); /* OBSOLETE: Made part of readline(). -- kjb */

#endif  /* __EDITLINE_H__ */
