/* Interactive helper for the tmux-driven wrapped-line test: read one line. */
#include <stdio.h>
#include "editline.h"
int main(void){ char *l = readline(""); printf("GOT[%s]", l ? l : ""); return 0; }
