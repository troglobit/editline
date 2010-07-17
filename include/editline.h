/* Minix editline */
#ifndef __EDITLINE_H__
#define __EDITLINE_H__

/* Display print 8-bit chars as `M-x' or as the actual 8-bit char?  (Default:1) */
extern int rl_meta_chars;

/* Assign these to get command completion, see cli.c for
 * example usage. */
extern char *(*rl_complete)(char *token, int *match);
extern int (*rl_list_possib)(char *token, char ***av);

/*
**  For compatibility with FSF readline.
*/
extern void rl_reset_terminal(char *p);
extern void rl_initialize(void);

extern char *readline(const char *prompt);
extern void add_history(char *line); /* OBSOLETE: Made part of readline(). -- kjb */

#endif  /* __EDITLINE_H__ */
