/* Custom CLI command completion.  */
#include "editline.h"
#include <string.h>

char *list[] = {
   "foo ", "bar ", "bsd ", "cli ", "ls ", "cd ", "malloc ", "tee ", NULL
};

/*
**  Attempt to complete the pathname, returning an allocated copy.
**  Fill in *unique if we completed it, or set it to 0 if ambiguous.
*/
char *my_rl_complete(char *token, int *match)
{
   int i;
   int index = -1;
   int matchlen = 0;
   int count = 0;

   for (i = 0; list[i]; i++)
   {
      int partlen = strlen (token); /* Part of token */

      if (!strncmp (list[i], token, partlen))
      {
         index = i;
         matchlen = partlen;
         count ++;
      }
   }

   if (count == 1)
   {
      *match = 1;
      return strdup (list[index] + matchlen);
   }

   return NULL;
}

/*
**  Return all possible completions.
*/
int my_rl_list_possib(char *token, char ***av)
{
   int i, num, total = 0;
   char **copy;
   
   for (num = 0; list[num]; num++)
      ;
   copy = (char **) malloc (num * sizeof(char *));
   for (i = 0; i < num; i++)
   {
      if (!strncmp (list[i], token, strlen (token)))
      {
         copy[total] = strdup (list[i]);
         total ++;
      }
   }
   *av = copy;

   return total;
}

int main(int ac __attribute__ ((unused)), char *av[] __attribute__ ((unused)))
{
   char *line;
   char	*prompt = "cli> ";

   /* Setup callbacks */
   rl_complete = &my_rl_complete;
   rl_list_possib = &my_rl_list_possib;

   while ((line = readline(prompt)) != NULL) {
      (void)printf("\t\t\t|%s|\n", line);
      free(line);
   }

   return 0;
}
