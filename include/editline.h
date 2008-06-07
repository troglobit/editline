/* Minix editline */
#ifndef __EDITLINE_H__
#define __EDITLINE_H__

/*
**  For compatibility with FSF readline.
*/
extern rl_reset_terminal(char *p);
extern void rl_initialize(void);

extern char *readline(const char *prompt);
extern void add_history(char *line); /* OBSOLETE: Made part of readline(). -- kjb */

#endif  /* __EDITLINE_H__ */
