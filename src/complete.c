/* History and file completion functions for editline library.
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

#define MAX_TOTAL_MATCHES (256 << sizeof(char *))

#ifndef HAVE_STRDUP
/* Return an allocated copy of a string. */
char *strdup(const char *p)
{
    char *new = malloc(sizeof(char) * strlen(p));

    if (new) {
        strcpy(new, p);
	return new;
    }

    return NULL;
}
#endif

/* Wrap strcmp() for qsort() */
static int compare(const void *p1, const void *p2)
{
    char **v1 = (char **)p1;
    char **v2 = (char **)p2;

    return strcmp(*v1, *v2);
}

/* Fill in *avp with an array of names that match file, up to its length.
 * Ignore . and .. . */
static int FindMatches(char *dir, char *file, char ***avp)
{
    char        **av;
    char        **new;
    char        *p;
    DIR         *dp;
    DIRENTRY    *ep;
    SIZE_T      ac;
    SIZE_T      len;
    SIZE_T      choices;
    SIZE_T      total;

    if ((dp = opendir(dir)) == NULL)
        return 0;

    av = NULL;
    ac = 0;
    len = strlen(file);
    choices = 0;
    total = 0;
    while ((ep = readdir(dp)) != NULL) {
        p = ep->d_name;
        if (p[0] == '.' && (p[1] == '\0' || (p[1] == '.' && p[2] == '\0')))
            continue;
        if (len && strncmp(p, file, len) != 0)
            continue;

        choices++;
        if ((total += strlen(p)) > MAX_TOTAL_MATCHES) {
            /* This is a bit too much. */
            while (ac > 0) free(av[--ac]);
            continue;
        }

        if ((ac % MEM_INC) == 0) {
	    new = malloc(sizeof(char *) * (ac + MEM_INC));
            if (!new) {
                total = 0;
                break;
            }
            if (ac) {
                memcpy(new, av, ac * sizeof(char **));
                free(av);
            }
            *avp = av = new;
        }

        if ((av[ac] = strdup(p)) == NULL) {
            if (ac == 0)
                free(av);
            total = 0;
            break;
        }
        ac++;
    }

    /* Clean up and return. */
    closedir(dp);
    if (total > MAX_TOTAL_MATCHES) {
        char many[sizeof(total) * 3];
        p = many + sizeof(many);
        *--p = '\0';
        while (choices > 0) {
           *--p = '0' + choices % 10;
           choices /= 10;
        }
        while (p > many + sizeof(many) - 8) *--p = ' ';
        if ((p = strdup(p)) != NULL) av[ac++] = p;
        if ((p = strdup("choices")) != NULL) av[ac++] = p;
    } else {
        if (ac)
            qsort(av, ac, sizeof (char **), compare);
    }

    return ac;
}

/* Split a pathname into allocated directory and trailing filename parts. */
static int SplitPath(char *path, char **dirpart, char **filepart)
{
    static char DOT[] = ".";
    char        *dpart;
    char        *fpart;

    if ((fpart = strrchr(path, '/')) == NULL) {
        if ((dpart = strdup(DOT)) == NULL)
            return -1;
        if ((fpart = strdup(path)) == NULL) {
            free(dpart);
            return -1;
        }
    }
    else {
        if ((dpart = strdup(path)) == NULL)
            return -1;
        dpart[fpart - path + 1] = '\0';
        if ((fpart = strdup(++fpart)) == NULL) {
            free(dpart);
            return -1;
        }
    }
    *dirpart = dpart;
    *filepart = fpart;

    return 0;
}

/* Attempt to complete the pathname, returning an allocated copy.
 * Fill in *unique if we completed it, or set it to 0 if ambiguous. */
char *default_rl_complete(char *pathname, int *unique)
{
    char        **av;
    char        *dir;
    char        *file;
    char        *new;
    char        *p;
    SIZE_T      ac;
    SIZE_T      end;
    SIZE_T      i;
    SIZE_T      j;
    SIZE_T      len;

    if (SplitPath(pathname, &dir, &file) < 0)
        return NULL;

    if ((ac = FindMatches(dir, file, &av)) == 0) {
        free(dir);
        free(file);

        return NULL;
    }

    p = NULL;
    len = strlen(file);
    if (ac == 1) {
        /* Exactly one match -- finish it off. */
        *unique = 1;
        j = strlen(av[0]) - len + 2;
	p = malloc(sizeof(char) * (j + 1));
        if (p) {
            memcpy(p, av[0] + len, j);
	    len = strlen(dir) + strlen(av[0]) + 2;
	    new = malloc(sizeof(char) * len);
            if (new) {
		snprintf(new, len, "%s/%s", dir, av[0]);
                rl_add_slash(new, p);
                free(new);
            }
        }
    }
    else {
        *unique = 0;
        if (len) {
            /* Find largest matching substring. */
            for (i = len, end = strlen(av[0]); i < end; i++)
                for (j = 1; j < ac; j++)
                    if (av[0][i] != av[j][i])
                        goto breakout;
  breakout:
            if (i > len) {
                j = i - len + 1;
		p = malloc(sizeof(char) * j);
                if (p) {
                    memcpy(p, av[0] + len, j);
                    p[j - 1] = '\0';
                }
            }
        }
    }

    /* Clean up and return. */
    free(dir);
    free(file);
    for (i = 0; i < ac; i++)
        free(av[i]);
    free(av);

    return p;
}

/* Return all possible completions. */
int default_rl_list_possib(char *pathname, char ***avp)
{
    char        *dir;
    char        *file;
    int         ac;

    if (SplitPath(pathname, &dir, &file) < 0)
        return 0;

    ac = FindMatches(dir, file, avp);
    free(dir);
    free(file);

    return ac;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "ellemtel"
 *  c-basic-offset: 4
 * End:
 */
