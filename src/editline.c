/* Main editing routines for editline library.
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

#include "editline.h"
#include <signal.h>
#include <errno.h>
#include <ctype.h>

/*
**  Manifest constants.
*/
#define SCREEN_COLS     80
#define SCREEN_ROWS     24
#define NO_ARG          (-1)
#define DEL             127
#define CTL(x)          ((x) & 0x1F)
#define ISCTL(x)        ((x) && (x) < ' ')
#define UNCTL(x)        ((x) + 64)
#define META(x)         ((x) | 0x80)
#define ISMETA(x)       ((x) & 0x80)
#define UNMETA(x)       ((x) & 0x7F)
#ifndef HIST_SIZE       /* Default to one line history, i.e. disabled. */
#define HIST_SIZE       1
#endif
#define SEPS "\"#$&'()*:;<=>?[\\]^`{|}~\n\t "

/*
**  Command status codes.
*/
typedef enum {
    CSdone, CSeof, CSmove, CSdispatch, CSstay, CSsignal
} el_status_t;

/*
**  The type of case-changing to perform.
*/
typedef enum {
    TOupper, TOlower
} el_case_t;

/*
**  Key to command mapping.
*/
typedef struct {
    int         Key;
    el_status_t (*Function)(void);
} el_keymap_t;

/*
**  Command history structure.
*/
typedef struct {
    int         Size;
    int         Pos;
    char       *Lines[HIST_SIZE];
} el_hist_t;

/*
**  Globals.
*/
int               rl_eof;
int               rl_erase;
int               rl_intr;
int               rl_kill;
int               rl_quit;
#ifdef CONFIG_SIGSTOP
int               rl_susp;
#endif

static char        NILSTR[] = "";
static const char *el_input = NILSTR;
static char       *Yanked;
static char       *Screen;
static char       NEWLINE[]= CRLF;
static el_hist_t  H;
static int        Repeat;
static int        old_point;
static int        el_push_back;
static int        el_pushed;
static int        el_intr_pending;
static el_keymap_t Map[];
static el_keymap_t MetaMap[];
static size_t     Length;
static size_t     ScreenCount;
static size_t     ScreenSize;
static char       *backspace;
static int        tty_cols;
static int        tty_rows;

int               el_no_echo = 0; /* e.g., under Emacs */
int               rl_point;
int               rl_mark;
int               rl_end;
int               rl_meta_chars = 0; /* Display 8-bit chars as the actual char(0) or as `M-x'(1)? */
char             *rl_line_buffer;
const char       *rl_prompt;
const char       *rl_readline_name;/* Set by calling program, for conditional parsing of ~/.inputrc - Not supported yet! */

/* User definable callbacks. */
char **(*rl_attempted_completion_function)(const char *token, int start, int end);

/* Declarations. */
static char     *editinput(void);
#ifdef CONFIG_USE_TERMCAP
extern char     *tgetstr(const char *, char **);
extern int      tgetent(char *, const char *);
extern int      tgetnum(const char *);
#endif

/*
**  TTY input/output functions.
*/

static void tty_flush(void)
{
    ssize_t res;

    if (ScreenCount) {
	if (!el_no_echo)
	    res = write (1, Screen, ScreenCount);
        ScreenCount = 0;
    }
}

static void tty_put(const char c)
{
    Screen[ScreenCount] = c;
    if (++ScreenCount >= ScreenSize - 1) {
        ScreenSize += SCREEN_INC;
        Screen = realloc(Screen, sizeof(char) * ScreenSize);
    }
}

static void tty_puts(const char *p)
{
    while (*p)
        tty_put(*p++);
}

static void tty_show(char c)
{
    if (c == DEL) {
        tty_put('^');
        tty_put('?');
    }
    else if (rl_meta_chars && ISMETA(c)) {
        tty_put('M');
        tty_put('-');
        tty_put(UNMETA(c));
    }
#if 0
   else if (ISCTL(c)) {
        tty_put('^');
        tty_put(UNCTL(c));
    }
#endif
    else {
        tty_put(c);
    }
}

static void tty_string(char *p)
{
    while (*p)
        tty_show(*p++);
}

static int tty_get(void)
{
    char c;
    int r;

    tty_flush();
    if (el_pushed) {
        el_pushed = 0;
        return el_push_back;
    }
    if (*el_input)
        return *el_input++;
    do
    {
        r = read(0, &c, 1);
    } while (r == -1 && errno == EINTR);

    return r == 1 ? c : EOF;
}

#define tty_back()       (backspace ? tty_puts(backspace) : tty_put('\b'))

static void tty_backn(int n)
{
    while (--n >= 0)
        tty_back();
}

static void tty_info(void)
{
    static int          init;
#ifdef CONFIG_USE_TERMCAP
    char               *term;
    char                buff[2048];
    char               *bp;
#endif
#ifdef TIOCGWINSZ
    struct winsize      W;
#endif

    if (init) {
#ifdef TIOCGWINSZ
        /* Perhaps we got resized. */
        if (ioctl(0, TIOCGWINSZ, &W) >= 0
         && W.ws_col > 0 && W.ws_row > 0) {
            tty_cols = (int)W.ws_col;
            tty_rows = (int)W.ws_row;
        }
#endif
        return;
    }
    init++;

    /* Initialize to faulty values to trigger fallback if nothing else works. */
    tty_cols = tty_rows = -1;
#ifdef CONFIG_USE_TERMCAP
    bp = buff;
    if ((term = getenv("TERM")) == NULL)
        term = "dumb";
    if (-1 != tgetent(buff, term)) {
	if ((backspace = tgetstr("le", &bp)) != NULL)
	    backspace = strdup(backspace);
	else
	    backspace = "\b";
	tty_cols = tgetnum("co");
	tty_rows = tgetnum("li");
    }
    /* Make sure to check width & rows and fallback to TIOCGWINSZ if available. */
#endif

    if (tty_cols <= 0 || tty_rows <= 0) {
#ifdef TIOCGWINSZ
       if (-1 != ioctl(0, TIOCGWINSZ, &W)) {
          tty_cols = (int)W.ws_col;
          tty_rows = (int)W.ws_row;
          return;
       }
#endif
       tty_cols = SCREEN_COLS;
       tty_rows = SCREEN_ROWS;
    }
}


/*
**  Print an array of words in columns.
*/
static void columns(int ac, char **av)
{
    char        *p;
    int         i;
    int         j;
    int         k;
    int         len;
    int         skip;
    int         longest;
    int         cols;

    /* Find longest name, determine column count from that. */
    for (longest = 0, i = 0; i < ac; i++)
        if ((j = strlen((char *)av[i])) > longest)
            longest = j;
    cols = tty_cols / (longest + 3);

    tty_puts(NEWLINE);
    for (skip = ac / cols + 1, i = 0; i < skip; i++) {
        for (j = i; j < ac; j += skip) {
            for (p = av[j], len = strlen((char *)p), k = len; --k >= 0; p++)
                tty_put(*p);
            if (j + skip < ac)
                while (++len < longest + 3)
                    tty_put(' ');
        }
        tty_puts(NEWLINE);
    }
}

static void reposition(void)
{
    int         i;
    char        *p;

    tty_put('\r');
    tty_puts(rl_prompt);
    for (i = rl_point, p = rl_line_buffer; --i >= 0; p++)
        tty_show(*p);
}

static void left(el_status_t Change)
{
    tty_back();
    if (rl_point) {
        if (ISCTL(rl_line_buffer[rl_point - 1]))
            tty_back();
        else if (rl_meta_chars && ISMETA(rl_line_buffer[rl_point - 1])) {
            tty_back();
            tty_back();
        }
    }
    if (Change == CSmove)
        rl_point--;
}

static void right(el_status_t Change)
{
    tty_show(rl_line_buffer[rl_point]);
    if (Change == CSmove)
        rl_point++;
}

static el_status_t ring_bell(void)
{
    tty_put('\07');
    tty_flush();
    return CSstay;
}

static el_status_t do_macro(int c)
{
    char name[4];

    name[0] = '_';
    name[1] = c;
    name[2] = '_';
    name[3] = '\0';

    if ((el_input = (char *)getenv((char *)name)) == NULL) {
        el_input = NILSTR;
        return ring_bell();
    }
    return CSstay;
}

static el_status_t do_forward(el_status_t move)
{
    int         i;
    char        *p;

    i = 0;
    do {
        p = &rl_line_buffer[rl_point];
        for ( ; rl_point < rl_end && (*p == ' ' || !isalnum(*p)); rl_point++, p++)
            if (move == CSmove)
                right(CSstay);

        for (; rl_point < rl_end && isalnum(*p); rl_point++, p++)
            if (move == CSmove)
                right(CSstay);

        if (rl_point == rl_end)
            break;
    } while (++i < Repeat);

    return CSstay;
}

static el_status_t do_case(el_case_t type)
{
    int         i;
    int         end;
    int         count;
    char        *p;

    do_forward(CSstay);
    if (old_point != rl_point) {
        if ((count = rl_point - old_point) < 0)
            count = -count;
        rl_point = old_point;
        if ((end = rl_point + count) > rl_end)
            end = rl_end;
        for (i = rl_point, p = &rl_line_buffer[i]; i < end; i++, p++) {
            if (type == TOupper) {
                if (islower(*p))
                    *p = toupper(*p);
            }
            else if (isupper(*p)) {
                *p = tolower(*p);
            }
            right(CSmove);
        }
    }
    return CSstay;
}

static el_status_t case_down_word(void)
{
    return do_case(TOlower);
}

static el_status_t case_up_word(void)
{
    return do_case(TOupper);
}

static void ceol(void)
{
    int         extras;
    int         i;
    char        *p;

    for (extras = 0, i = rl_point, p = &rl_line_buffer[i]; i <= rl_end; i++, p++) {
        tty_put(' ');
        if (ISCTL(*p)) {
            tty_put(' ');
            extras++;
        }
        else if (rl_meta_chars && ISMETA(*p)) {
            tty_put(' ');
            tty_put(' ');
            extras += 2;
        }
    }

    for (i += extras; i > rl_point; i--)
        tty_back();
}

static void clear_line(void)
{
    rl_point = -(int)strlen(rl_prompt);
    tty_put('\r');
    ceol();
    rl_point = 0;
    rl_end = 0;
    rl_line_buffer[0] = '\0';
}

static el_status_t insert_string(const char *p)
{
    size_t      len;
    int         i;
    char        *new;
    char        *q;

    len = strlen(p);
    if (rl_end + len >= Length) {
	new = malloc(sizeof(char) * (Length + len + MEM_INC));
        if (!new)
            return CSstay;
        if (Length) {
            memcpy(new, rl_line_buffer, Length);
            free(rl_line_buffer);
        }
        rl_line_buffer = new;
        Length += len + MEM_INC;
    }

    for (q = &rl_line_buffer[rl_point], i = rl_end - rl_point; --i >= 0; )
        q[len + i] = q[i];
    memcpy(&rl_line_buffer[rl_point], p, len);
    rl_end += len;
    rl_line_buffer[rl_end] = '\0';
    tty_string(&rl_line_buffer[rl_point]);
    rl_point += len;

    return rl_point == rl_end ? CSstay : CSmove;
}

static el_status_t redisplay(void)
{
    tty_puts(NEWLINE); /* XXX: Use "\r\e[K" to get really neat effect on ANSI capable terminals. */
    tty_puts(rl_prompt);
    tty_string(rl_line_buffer);
    return CSmove;
}

static el_status_t toggle_meta_mode(void)
{
    rl_meta_chars = ! rl_meta_chars;
    return redisplay();
}


static const char *next_hist(void)
{
    return H.Pos >= H.Size - 1 ? NULL : H.Lines[++H.Pos];
}

static const char *prev_hist(void)
{
    return H.Pos == 0 ? NULL : H.Lines[--H.Pos];
}

static el_status_t do_insert_hist(const char *p)
{
    if (p == NULL)
        return ring_bell();
    rl_point = 0;
    reposition();
    ceol();
    rl_end = 0;
    return insert_string(p);
}

static el_status_t do_hist(const char *(*move)(void))
{
    const char *p;
    int i;

    i = 0;
    do {
        if ((p = (*move)()) == NULL)
            return ring_bell();
    } while (++i < Repeat);
    return do_insert_hist(p);
}

static el_status_t h_next(void)
{
    return do_hist(next_hist);
}

static el_status_t h_prev(void)
{
    return do_hist(prev_hist);
}

static el_status_t h_first(void)
{
    return do_insert_hist(H.Lines[H.Pos = 0]);
}

static el_status_t h_last(void)
{
    return do_insert_hist(H.Lines[H.Pos = H.Size - 1]);
}

/*
**  Return zero if pat appears as a substring in text.
*/
static int substrcmp(const char *text, const char *pat, size_t len)
{
    char c;

    if ((c = *pat) == '\0')
        return *text == '\0';
    for ( ; *text; text++)
        if (*text == c && strncmp(text, pat, len) == 0)
            return 0;
    return 1;
}

static const char *search_hist(const char *search, const char *(*move)(void))
{
    static char *old_search;
    int         len;
    int         pos;
    int         (*match)(const char *s1, const char *s2, size_t n);
    const char *pat;

    /* Save or get remembered search pattern. */
    if (search && *search) {
        if (old_search)
            free(old_search);
        old_search = strdup(search);
    }
    else {
        if (old_search == NULL || *old_search == '\0')
            return NULL;
        search = old_search;
    }

    /* Set up pattern-finder. */
    if (*search == '^') {
        match = strncmp;
        pat = search + 1;
    }
    else {
        match = substrcmp;
        pat = search;
    }
    len = strlen(pat);

    for (pos = H.Pos; (*move)() != NULL; )
        if (match(H.Lines[H.Pos], pat, len) == 0)
            return H.Lines[H.Pos];
    H.Pos = pos;

    return NULL;
}

static el_status_t h_search(void)
{
    static int Searching;
    const char *old_prompt;
    const char *(*move)(void);
    const char *p;

    if (Searching)
        return ring_bell();
    Searching = 1;

    clear_line();
    old_prompt = rl_prompt;
    rl_prompt = "Search: ";
    tty_puts(rl_prompt);
    move = Repeat == NO_ARG ? prev_hist : next_hist;
    p = editinput();
    rl_prompt = old_prompt;
    Searching = 0;
    tty_puts(rl_prompt);
    if (p == NULL && el_intr_pending > 0) {
        el_intr_pending = 0;
        clear_line();
        return redisplay();
    }
    p = search_hist(p, move);
    clear_line();
    if (p == NULL) {
        ring_bell();
        return redisplay();
    }
    return do_insert_hist(p);
}

static el_status_t fd_char(void)
{
    int         i;

    i = 0;
    do {
        if (rl_point >= rl_end)
            break;
        right(CSmove);
    } while (++i < Repeat);
    return CSstay;
}

static void save_yank(int begin, int i)
{
    if (Yanked) {
        free(Yanked);
        Yanked = NULL;
    }

    if (i < 1)
        return;

    Yanked = malloc(sizeof(char) * (i + 1));
    if (Yanked) {
        memcpy(Yanked, &rl_line_buffer[begin], i);
        Yanked[i] = '\0';
    }
}

static el_status_t delete_string(int count)
{
    int         i;
    char        *p;

    if (count <= 0 || rl_end == rl_point)
        return ring_bell();

    if (count == 1 && rl_point == rl_end - 1) {
        /* Optimize common case of delete at end of line. */
        rl_end--;
        p = &rl_line_buffer[rl_point];
        i = 1;
        tty_put(' ');
        if (ISCTL(*p)) {
            i = 2;
            tty_put(' ');
        }
        else if (rl_meta_chars && ISMETA(*p)) {
            i = 3;
            tty_put(' ');
            tty_put(' ');
        }
        tty_backn(i);
        *p = '\0';
        return CSmove;
    }
    if (rl_point + count > rl_end && (count = rl_end - rl_point) <= 0)
        return CSstay;

    if (count > 1)
        save_yank(rl_point, count);

    for (p = &rl_line_buffer[rl_point], i = rl_end - (rl_point + count) + 1; --i >= 0; p++)
        p[0] = p[count];
    ceol();
    rl_end -= count;
    tty_string(&rl_line_buffer[rl_point]);
    return CSmove;
}

static el_status_t bk_char(void)
{
    int         i;

    i = 0;
    do {
        if (rl_point == 0)
            break;
        left(CSmove);
    } while (++i < Repeat);

    return CSstay;
}

static el_status_t bk_del_char(void)
{
    int         i;

    i = 0;
    do {
        if (rl_point == 0)
            break;
        left(CSmove);
    } while (++i < Repeat);

    return delete_string(i);
}

static el_status_t kill_line(void)
{
    int         i;

    if (Repeat != NO_ARG) {
        if (Repeat < rl_point) {
            i = rl_point;
            rl_point = Repeat;
            reposition();
            delete_string(i - rl_point);
        }
        else if (Repeat > rl_point) {
            right(CSmove);
            delete_string(Repeat - rl_point - 1);
        }
        return CSmove;
    }

    save_yank(rl_point, rl_end - rl_point);
    rl_line_buffer[rl_point] = '\0';
    ceol();
    rl_end = rl_point;
    return CSstay;
}

static el_status_t insert_char(int c)
{
    el_status_t      s;
    char        buff[2];
    char        *p;
    char        *q;
    int         i;

    if (Repeat == NO_ARG || Repeat < 2) {
        buff[0] = c;
        buff[1] = '\0';
        return insert_string(buff);
    }

    p = malloc(sizeof(char) * (Repeat + 1));
    if (!p)
        return CSstay;

    for (i = Repeat, q = p; --i >= 0; )
        *q++ = c;
    *q = '\0';
    Repeat = 0;
    s = insert_string(p);
    free(p);

    return s;
}

static el_status_t beg_line(void)
{
    if (rl_point) {
        rl_point = 0;
        return CSmove;
    }
    return CSstay;
}

static el_status_t end_line(void)
{
    if (rl_point != rl_end) {
        rl_point = rl_end;
        return CSmove;
    }
    return CSstay;
}

static el_status_t del_char(void)
{
    return delete_string(Repeat == NO_ARG ? 1 : Repeat);
}

static el_status_t meta(void)
{
    int c;
    el_keymap_t              *kp;

    if ((c = tty_get()) == EOF)
        return CSeof;

#ifdef CONFIG_ANSI_ARROWS
    /* Also include VT-100 arrows. */
    if (c == '[' || c == 'O') {
        c = tty_get();
        switch (c) {
	    case EOF:  return CSeof;
	    case '2':  tty_get(); return CSstay;     /* Insert */
	    case '3':  tty_get(); return del_char(); /* Delete */
	    case '5':  tty_get(); return CSstay;     /* PgUp */
	    case '6':  tty_get(); return CSstay;     /* PgDn */
	    case 'A':  return h_prev();              /* Up */
	    case 'B':  return h_next();              /* Down */
	    case 'C':  return fd_char();             /* Left */
	    case 'D':  return bk_char();             /* Right */
	    case 'F':  return end_line();            /* End */
	    case 'H':  return beg_line();            /* Home */
	    default:                                 /* Fall through */
		break;
        }

	return ring_bell();
    }
#endif /* CONFIG_ANSI_ARROWS */

    if (isdigit(c)) {
        for (Repeat = c - '0'; (c = tty_get()) != EOF && isdigit(c); )
            Repeat = Repeat * 10 + c - '0';
        el_pushed = 1;
        el_push_back = c;
        return CSstay;
    }

    if (isupper(c))
        return do_macro(c);
    for (kp = MetaMap; kp->Function; kp++)
        if (kp->Key == c)
            return (*kp->Function)();

    return ring_bell();
}

static el_status_t emacs(int c)
{
    el_status_t  s;
    el_keymap_t *kp;

    /* Save point before interpreting input character 'c'. */
    old_point = rl_point;

    /* This test makes it impossible to enter eight-bit characters when
     * meta-char mode is enabled. */
    if (rl_meta_chars && ISMETA(c)) {
        el_pushed = 1;
        el_push_back = UNMETA(c);
        return meta();
    }

    for (kp = Map; kp->Function; kp++) {
        if (kp->Key == c)
            break;
    }
    s = kp->Function ? (*kp->Function)() : insert_char(c);
    if (!el_pushed) {
        /* No pushback means no repeat count; hacky, but true. */
        Repeat = NO_ARG;
    }

    return s;
}

static el_status_t tty_special(int c)
{
    if (rl_meta_chars && ISMETA(c))
        return CSdispatch;

    if (c == rl_erase || c == DEL)
        return bk_del_char();
    if (c == rl_kill) {
        if (rl_point != 0) {
            rl_point = 0;
            reposition();
        }
        Repeat = NO_ARG;
        return kill_line();
    }
    if (c == rl_eof && rl_point == 0 && rl_end == 0)
        return CSeof;
    if (c == rl_intr) {
        el_intr_pending = SIGINT;
        return CSsignal;
    }
    if (c == rl_quit) {
        el_intr_pending = SIGQUIT;
        return CSeof;
    }
#ifdef CONFIG_SIGSTOP
    if (c == rl_susp) {
        el_intr_pending = SIGTSTP;
        return CSsignal;
    }
#endif

    return CSdispatch;
}

static char *editinput(void)
{
    int c;

    Repeat = NO_ARG;
    old_point = rl_point = rl_mark = rl_end = 0;
    rl_line_buffer[0] = '\0';

    el_intr_pending = -1;
    while ((c = tty_get()) != EOF) {
        switch (tty_special(c)) {
	    case CSdone:
		return rl_line_buffer;

	    case CSeof:
		return NULL;

	    case CSsignal:
		return (char *)"";

	    case CSmove:
		reposition();
		break;

	    case CSdispatch:
		switch (emacs(c)) {
		    case CSdone:
			return rl_line_buffer;

		    case CSeof:
			return NULL;

		    case CSsignal:
			return (char *)"";

		    case CSmove:
			reposition();
			break;

		    case CSdispatch:
		    case CSstay:
			break;
		}
		break;

	    case CSstay:
		break;
        }
    }
    return NULL;
}

static void hist_add(char *p)
{
    int i;

    if ((p = (char *)strdup((char *)p)) == NULL)
        return;
    if (H.Size < HIST_SIZE)
        H.Lines[H.Size++] = p;
    else {
        free(H.Lines[0]);
        for (i = 0; i < HIST_SIZE - 1; i++)
            H.Lines[i] = H.Lines[i + 1];
        H.Lines[i] = p;
    }
    H.Pos = H.Size - 1;
}

static char *read_redirected(void)
{
    int         size = MEM_INC;
    char        *p;
    char        *line;
    char        *end;

    p = line = malloc(sizeof(char) * size);
    if (!p)
	return NULL;

    end = p + size;
    while (1) {
        if (p == end) {
            int oldpos = end - line;

            size += MEM_INC;
            p = line = realloc(line, sizeof(char) * size);
	    if (!p)
		return NULL;
            end = p + size;

            p += oldpos;        /* Continue where we left off... */
        }
        if (read(0, p, 1) <= 0) {
            /* Ignore "incomplete" lines at EOF, just like we do for a tty. */
            free(line);
            return NULL;
        }
        if (*p == '\n')
            break;
        p++;
    }
    *p = '\0';

    return line;
}

/* For compatibility with FSF readline. */
void rl_reset_terminal(char *p __attribute__((__unused__)))
{
}

void rl_initialize(void)
{
    if (!rl_prompt)
	rl_prompt = "? ";
}

char *readline(const char *prompt)
{
    char        *line;

    /* Unless called by the user already. */
    rl_initialize();

    if (!isatty(0)) {
        tty_flush();
        return read_redirected();
    }

    if (!rl_line_buffer) {
        Length = MEM_INC;
	rl_line_buffer = malloc(sizeof(char) * Length);
        if (!rl_line_buffer)
            return NULL;
    }

    tty_info();
    rl_ttyset(0);
    hist_add(NILSTR);
    ScreenSize = SCREEN_INC;
    Screen = malloc(sizeof(char) * ScreenSize);
    if (!Screen)
	return NULL;

    rl_prompt = prompt ? prompt : NILSTR;
    if (el_no_echo) {
	int old = el_no_echo;
	el_no_echo = 0;
	tty_puts(rl_prompt);
	tty_flush();
	el_no_echo = old;
    } else {
	tty_puts(rl_prompt);
    }

    line = editinput();
    if (line) {
        line = strdup(line);
        tty_puts(NEWLINE);
        tty_flush();
    }

    rl_ttyset(1);
    free(Screen);
    free(H.Lines[--H.Size]);

    if (line != NULL && *line != '\0'
#ifdef CONFIG_UNIQUE_HISTORY
        && !(H.Pos && strcmp(line, H.Lines[H.Pos - 1]) == 0)
#endif
        && !(H.Size && strcmp(line, H.Lines[H.Size - 1]) == 0)
    ) {
        hist_add(line);
    }

    if (el_intr_pending > 0) {
	int s = el_intr_pending;
        el_intr_pending = 0;
        kill(getpid(), s);
    }

    return line;
}

void add_history(char *p __attribute__ ((unused)))
{
#ifdef obsolete         /* Made part of readline(). -- kjb */
    if (p == NULL || *p == '\0')
        return;

#ifdef CONFIG_UNIQUE_HISTORY
    if (H.Pos && strcmp(p, (char *) H.Lines[H.Pos - 1]) == 0)
        return;
#endif
    if (H.Size && strcmp(p, (char *) H.Lines[H.Size - 1]) == 0)
        return;
    hist_add((char *)p);
#endif
}


/*
**  Move back to the beginning of the current word and return an
**  allocated copy of it.
*/
static char *find_word(void)
{
    char        *p, *q;
    char        *new;
    size_t      len;

    p = &rl_line_buffer[rl_point];
    while (p > rl_line_buffer) {
        p--;
        if (p > rl_line_buffer && p[-1] == '\\') {
            p--;
        } else {
            if (strchr(SEPS, (char) *p) != NULL) {
                p++;
                break;
            }
        }
    }

    len = rl_point - (p - rl_line_buffer) + 1;
    new = malloc(sizeof(char) * len);
    if (!new)
        return NULL;

    q = new;
    while (p < &rl_line_buffer[rl_point]) {
        if (*p == '\\') {
            if (++p == &rl_line_buffer[rl_point])
		break;
        }
        *q++ = *p++;
    }
    *q = '\0';

    return new;
}

static el_status_t c_possible(void)
{
    char        **av;
    char        *word;
    int         ac;

    word = find_word();
    ac = rl_list_possib(word, &av);
    if (word)
        free(word);
    if (ac) {
        columns(ac, av);
        while (--ac >= 0)
            free(av[ac]);
        free(av);

        return CSmove;
    }

    return ring_bell();
}

static el_status_t c_complete(void)
{
    char        *p, *q;
    char        *word, *new;
    size_t      len;
    int         unique;
    el_status_t s = 0;

    word = find_word();
    p = (char *)rl_complete((char *)word, &unique);
    if (word)
        free(word);
    if (p) {
        len = strlen((char *)p);
        word = p;
        new = q = malloc(sizeof(char) * (2 * len + 1));
	if (!new)
	    return CSstay;
        while (*p) {
            if ((*p < ' ' || strchr(SEPS, (char) *p) != NULL)
                                && (!unique || p[1] != 0)) {
                *q++ = '\\';
            }
            *q++ = *p++;
        }
        *q = '\0';
        free(word);
        if (len > 0) {
            s = insert_string(new);
#ifdef CONFIG_ANNOYING_NOISE
            if (!unique)
                ring_bell();
#endif
        }
        free(new);
        if (len > 0)
	    return s;
    }
    return c_possible();
}

static el_status_t accept_line(void)
{
    rl_line_buffer[rl_end] = '\0';
    return CSdone;
}

static el_status_t transpose(void)
{
    char        c;

    if (rl_point) {
        if (rl_point == rl_end)
            left(CSmove);
        c = rl_line_buffer[rl_point - 1];
        left(CSstay);
        rl_line_buffer[rl_point - 1] = rl_line_buffer[rl_point];
        tty_show(rl_line_buffer[rl_point - 1]);
        rl_line_buffer[rl_point++] = c;
        tty_show(c);
    }
    return CSstay;
}

static el_status_t quote(void)
{
    int c;

    return (c = tty_get()) == EOF ? CSeof : insert_char((int)c);
}

static el_status_t mk_set(void)
{
    rl_mark = rl_point;
    return CSstay;
}

static el_status_t exchange(void)
{
    int c;

    if ((c = tty_get()) != CTL('X'))
        return c == EOF ? CSeof : ring_bell();

    if ((c = rl_mark) <= rl_end) {
        rl_mark = rl_point;
        rl_point = c;
        return CSmove;
    }
    return CSstay;
}

static el_status_t yank(void)
{
    if (Yanked && *Yanked)
        return insert_string(Yanked);
    return CSstay;
}

static el_status_t copy_region(void)
{
    if (rl_mark > rl_end)
        return ring_bell();

    if (rl_point > rl_mark)
        save_yank(rl_mark, rl_point - rl_mark);
    else
        save_yank(rl_point, rl_mark - rl_point);

    return CSstay;
}

static el_status_t move_to_char(void)
{
    int c;
    int                 i;
    char                *p;

    if ((c = tty_get()) == EOF)
        return CSeof;
    for (i = rl_point + 1, p = &rl_line_buffer[i]; i < rl_end; i++, p++)
        if (*p == c) {
            rl_point = i;
            return CSmove;
        }
    return CSstay;
}

static el_status_t fd_word(void)
{
    return do_forward(CSmove);
}

static el_status_t fd_kill_word(void)
{
    int         i;

    do_forward(CSstay);
    if (old_point != rl_point) {
        i = rl_point - old_point;
        rl_point = old_point;
        return delete_string(i);
    }
    return CSstay;
}

static el_status_t bk_word(void)
{
    int         i;
    char        *p;

    i = 0;
    do {
        for (p = &rl_line_buffer[rl_point]; p > rl_line_buffer && !isalnum(p[-1]); p--)
            left(CSmove);

        for (; p > rl_line_buffer && p[-1] != ' ' && isalnum(p[-1]); p--)
            left(CSmove);

        if (rl_point == 0)
            break;
    } while (++i < Repeat);

    return CSstay;
}

static el_status_t bk_kill_word(void)
{
    bk_word();
    if (old_point != rl_point)
        return delete_string(old_point - rl_point);

    return CSstay;
}

static int argify(char *line, char ***avp)
{
    char       *c;
    char      **p;
    char      **new;
    int         ac;
    int         i;

    i = MEM_INC;
    *avp = p = malloc(sizeof(char *) * i);
    if (!p)
         return 0;

    for (c = line; isspace(*c); c++)
        continue;
    if (*c == '\n' || *c == '\0')
        return 0;

    for (ac = 0, p[ac++] = c; *c && *c != '\n'; ) {
        if (isspace(*c)) {
            *c++ = '\0';
            if (*c && *c != '\n') {
                if (ac + 1 == i) {
                    new = malloc(sizeof(char *) * (i + MEM_INC));
                    if (!new) {
                        p[ac] = NULL;
                        return ac;
                    }
                    memcpy(new, p, i * sizeof(char **));
                    i += MEM_INC;
                    free(p);
                    *avp = p = new;
                }
                p[ac++] = c;
            }
        }
        else
            c++;
    }
    *c = '\0';
    p[ac] = NULL;

    return ac;
}

static el_status_t last_argument(void)
{
    char      **av = NULL;
    char       *p;
    el_status_t s;
    int         ac;

    if (H.Size == 1 || (p = (char *)H.Lines[H.Size - 2]) == NULL)
        return ring_bell();

    if ((p = strdup(p)) == NULL)
        return CSstay;
    ac = argify(p, &av);

    if (Repeat != NO_ARG)
        s = Repeat < ac ? insert_string(av[Repeat]) : ring_bell();
    else
        s = ac ? insert_string(av[ac - 1]) : CSstay;

    if (av)
        free(av);
    free(p);

    return s;
}

static el_keymap_t Map[] = {
    {   CTL('@'),       mk_set          },
    {   CTL('A'),       beg_line        },
    {   CTL('B'),       bk_char         },
    {   CTL('D'),       del_char        },
    {   CTL('E'),       end_line        },
    {   CTL('F'),       fd_char         },
    {   CTL('G'),       ring_bell       },
    {   CTL('H'),       bk_del_char     },
    {   CTL('I'),       c_complete      },
    {   CTL('J'),       accept_line     },
    {   CTL('K'),       kill_line       },
    {   CTL('L'),       redisplay       },
    {   CTL('M'),       accept_line     },
    {   CTL('N'),       h_next          },
    {   CTL('O'),       ring_bell       },
    {   CTL('P'),       h_prev          },
    {   CTL('Q'),       ring_bell       },
    {   CTL('R'),       h_search        },
    {   CTL('S'),       ring_bell       },
    {   CTL('T'),       transpose       },
    {   CTL('U'),       ring_bell       },
    {   CTL('V'),       quote           },
    {   CTL('W'),       bk_kill_word    },
    {   CTL('X'),       exchange        },
    {   CTL('Y'),       yank            },
    {   CTL('Z'),       end_line        },
    {   CTL('['),       meta            },
    {   CTL(']'),       move_to_char    },
    {   CTL('^'),       ring_bell       },
    {   CTL('_'),       ring_bell       },
    {   0,              NULL            }
};

static el_keymap_t   MetaMap[]= {
    {   CTL('H'),       bk_kill_word    },
    {   DEL,            bk_kill_word    },
    {   ' ',            mk_set          },
    {   '.',            last_argument   },
    {   '<',            h_first         },
    {   '>',            h_last          },
    {   '?',            c_possible      },
    {   'b',            bk_word         },
    {   'd',            fd_kill_word    },
    {   'f',            fd_word         },
    {   'l',            case_down_word  },
    {   'm',            toggle_meta_mode },
    {   'u',            case_up_word    },
    {   'y',            yank            },
    {   'w',            copy_region     },
    {   0,              NULL            }
};

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "ellemtel"
 *  c-basic-offset: 4
 * End:
 */
