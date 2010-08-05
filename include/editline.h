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

/* Command status codes. */
typedef enum {
    CSdone, CSeof, CSmove, CSdispatch, CSstay, CSsignal
} el_status_t;

/* Editline specific types, despite rl_ prefix.  From Heimdal project. */
typedef char* rl_complete_func_t(char*, int*);
typedef int rl_list_possib_func_t(char*, char***);
typedef el_status_t el_keymap_func_t(void);
typedef int  rl_hook_func_t(void);
typedef int  rl_getc_func_t(void);
typedef void rl_voidfunc_t(void);
typedef void rl_vintfunc_t(int);

/* Display 8-bit chars "as-is" or as `M-x'? Toggle with M-m. (Default:0 - "as-is") */
extern int rl_meta_chars;

/* Editline specific functions. */
void el_bind_key_in_metamap(char c, el_keymap_func_t func);

/* For compatibility with FSF readline. */
extern int         rl_point;
extern int         rl_mark;
extern int         rl_end;
extern char       *rl_line_buffer;
extern const char *rl_readline_name;
extern FILE       *rl_instream;  /* The stdio stream from which input is read. Defaults to stdin if NULL - Not supported yet! */
extern FILE       *rl_outstream; /* The stdio stream to which output is flushed. Defaults to stdout if NULL - Not supported yet! */
extern int         el_no_echo;   /* e.g under emacs, don't echo except prompt */
extern int         el_hist_size; /* size of history scrollback buffer, default: 15 */

extern void rl_initialize(void);
extern void rl_reset_terminal(char *p);

extern char *readline(const char *prompt);
extern void add_history(const char *line);

extern int read_history(const char *filename);
extern int write_history(const char *filename);

rl_complete_func_t    *rl_set_complete_func(rl_complete_func_t *func);
rl_list_possib_func_t *rl_set_list_possib_func(rl_list_possib_func_t *func);

void rl_prep_terminal(int meta_flag);
void rl_deprep_terminal(void);

int rl_getc(void);

#endif  /* __EDITLINE_H__ */
