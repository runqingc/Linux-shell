#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
    printf("\n\n------------------------\nHere 's msh.c\n");

    // should it be []? unless I cannot modify it
    // should I strip the space at the end of each job string?
    char cmd_line[]="; ls -la &  ; cat  file.txt   ";
    int type;
    char *job;
    job = parse_tok(cmd_line,&type);
    printf("job=%s, type=%d\n",job,type); // prints job=ls -la type=0
    job = parse_tok(NULL,&type);
    printf("job=%s, type=%d\n",job,type); // prints job=cd .. type=1
    job = parse_tok(NULL,&type);
    printf("job=%s, type=%d\n",job,type); // prints job=cat file.txt type=1
    job = parse_tok(NULL,&type); // job = NULL  type = -1
    job = parse_tok(NULL,&type); // Still job = NULL  type = -1

    return 0;
}

