/*  $Revision: 5 $
**
**  Unix system-dependant routines for editline library.
*/
#include "editline.h"

#if defined(HAVE_TCGETATTR)
#include <termios.h>

void rl_ttyset(int Reset)
{
    static struct termios       old;
    struct termios              new;

    if (Reset == 0) {
        if (tcgetattr(0, &old) < 0) perror("tcgetattr");
        rl_erase = old.c_cc[VERASE];
        rl_kill = old.c_cc[VKILL];
        rl_eof = old.c_cc[VEOF];
        rl_intr = old.c_cc[VINTR];
        rl_quit = old.c_cc[VQUIT];
#ifdef DO_SIGTSTP
        rl_susp = old.c_cc[VSUSP];
#endif

        new = old;
        new.c_lflag &= ~(ECHO | ICANON | ISIG);
        new.c_iflag &= ~(ISTRIP | INPCK);
        new.c_cc[VMIN] = 1;
        new.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSADRAIN, &new) < 0) perror("tcsetattr");
    } else {
        (void)tcsetattr(0, TCSADRAIN, &old);
    }
}

#elif defined(HAVE_TERMIO_H)
#include <termio.h>

void rl_ttyset(int Reset)
{
    static struct termio        old;
    struct termio               new;

    if (Reset == 0) {
        (void)ioctl(0, TCGETA, &old);
        rl_erase = old.c_cc[VERASE];
        rl_kill = old.c_cc[VKILL];
        rl_eof = old.c_cc[VEOF];
        rl_intr = old.c_cc[VINTR];
        rl_quit = old.c_cc[VQUIT];
#ifdef DO_SIGTSTP
        rl_susp = old.c_cc[VSUSP];
#endif

        new = old;
        new.c_lflag &= ~(ECHO | ICANON | ISIG);
        new.c_iflag &= ~(ISTRIP | INPCK);
        new.c_cc[VMIN] = 1;
        new.c_cc[VTIME] = 0;
        (void)ioctl(0, TCSETAW, &new);
    } else {
        (void)ioctl(0, TCSETAW, &old);
    }
}

#else /* Neither HAVE_TERMIO_H or HAVE_TCGETATTR */
#include <sgtty.h>

void rl_ttyset(int Reset)
{
    static struct sgttyb        old_sgttyb;
    static struct tchars        old_tchars;
    struct sgttyb               new_sgttyb;
    struct tchars               new_tchars;
#ifdef DO_SIGTSTP
    struct ltchars              old_ltchars;
#endif

    if (Reset == 0) {
        (void)ioctl(0, TIOCGETP, &old_sgttyb);
        rl_erase = old_sgttyb.sg_erase;
        rl_kill = old_sgttyb.sg_kill;

        (void)ioctl(0, TIOCGETC, &old_tchars);
        rl_eof = old_tchars.t_eofc;
        rl_intr = old_tchars.t_intrc;
        rl_quit = old_tchars.t_quitc;

#ifdef DO_SIGTSTP
        (void)ioctl(0, TIOCGLTC, &old_ltchars);
        rl_susp = old_ltchars.t_suspc;
#endif

        new_sgttyb = old_sgttyb;
        new_sgttyb.sg_flags &= ~ECHO;
        new_sgttyb.sg_flags |= RAW;
#ifdef PASS8
        new_sgttyb.sg_flags |= PASS8;
#endif
        (void)ioctl(0, TIOCSETP, &new_sgttyb);

        new_tchars = old_tchars;
        new_tchars.t_intrc = -1;
        new_tchars.t_quitc = -1;
        (void)ioctl(0, TIOCSETC, &new_tchars);
    } else {
        (void)ioctl(0, TIOCSETP, &old_sgttyb);
        (void)ioctl(0, TIOCSETC, &old_tchars);
    }
}
#endif /* Neither HAVE_TERMIO_H or HAVE_TCGETATTR */

void rl_add_slash(char *path, char *p)
{
    struct stat Sb;

    if (stat(path, &Sb) >= 0)
        (void)strcat(p, S_ISDIR(Sb.st_mode) ? "/" : " ");
}

/*
 * $PchId: sysunix.c,v 1.4 1996/02/22 21:16:56 philip Exp $
 */
