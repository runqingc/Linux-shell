#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

const char* EXIT = "exit";

void test_parse_tok();
void test_separate_args();
void test_evaluate();




int main(int argc, char *argv[]){

    // uncomment this to see my own test result
    // test_parse_tok();
    // test_separate_args();
    // test_evaluate();

    int opt, result;
    int line=0, history=0, job=0;

    while((opt = getopt(argc, argv, "j:s:l:")) != -1){

        switch (opt) {

            case 'l':
                result = sscanf(optarg, "%d", &line);
                if(result!=1 || line<=0){
                    printf("usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    return 0;
                }
                break;
            case 's':
                result = sscanf(optarg, "%d", &history);
                if(result!=1 || history<=0){
                    printf("usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    return 0;
                }
                break;
            case 'j':
                result = sscanf(optarg, "%d", &job);
                if(result!=1 || job<=0){
                    printf("usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    return 0;
                }
                break;
            default:
                printf("usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                return 0;
        }

    }

    // if passes check, allocate a new shell
    msh_t* shell = alloc_shell(job, line, history);

    // run shell
    printf("msh> ");


    // read from command line
    char *cmd_line = NULL;
    long int len = 0;
    long nRead = getline(&cmd_line, &len, stdin);
    // replace the '\n' to '\0'
    cmd_line[strcspn(cmd_line, "\n")] = '\0';
    while ( nRead != -1) {
        if(strcmp(cmd_line, EXIT)==0){
            if(cmd_line) free(cmd_line);
            cmd_line = NULL;
            break;
        }else{
            // parse, then evaluate each job
            int type;
            char *cur_job;
            cur_job = parse_tok(cmd_line, &type);
            // printf("cur_job=%s\n", cur_job);
            evaluate(shell, cur_job);
            do{
                cur_job = parse_tok(NULL, &type);
                // printf("cur_job=%s\n", cur_job);
                evaluate(shell, cur_job);
            }while(cur_job);
            
        }
        printf("msh> ");
        // Done using line so I must free it!!
        if(cmd_line) free(cmd_line);
        //Make sure to reset line back to null for the next line
        cmd_line = NULL;
        nRead = getline(&cmd_line, &len, stdin);
        cmd_line[strcspn(cmd_line, "\n")] = '\0';
    }



    return 0;
}



void test_parse_tok(){
    printf("\ntest_parse_tok: \n");
    // should it be []? unless I cannot modify it
    // should I strip the space at the end of each job string?
    char cmd_line[]="ls -la & cd .. ; cat file.txt";
    int type;
    char *job;
    job = parse_tok(cmd_line,&type);
    printf("job=%s, type=%d\n",job,type); // prints job=ls -la type=0
    job = parse_tok(NULL,&type);
    printf("job=%s, type=%d\n",job,type); // prints job=cd .. type=1
    job = parse_tok(NULL,&type);
    printf("job=%s, type=%d\n",job,type); // prints job=cat file.txt type=1
    job = parse_tok(NULL,&type); // job = NULL  type = -1
    printf("job=%s, type=%d\n",job,type);
    job = parse_tok(NULL,&type); // Still job = NULL  type = -1
    printf("job=%s, type=%d\n",job,type);
}

void test_separate_args(){
    printf("\ntest_separate_args: \n");
    char cmd_line[]="\0";
    bool is_builtin_temp = true;
    char **argv;
    int argc;
    argv = separate_args(cmd_line,&argc, &is_builtin_temp);
    printf("%s",argv[0]); // prints ls
    printf("%s",argv[1]); // prints -la
    printf("%s",argv[2]); // prints  /mpcs51082-aut23
    // note there is an argv[3] that should contain NULL
    printf("%d", argc); // prints 3
    argv = separate_args("",&argc, &is_builtin_temp);  // argv = NULL
}

void test_evaluate(){
    printf("\ntest_evaluate: \n");
    char cmd_line[]="  ls -la   /mpcs51082-aut23 ";
    printf("evaluate line: %s\n", cmd_line);
    // allocate a new shell
    msh_t* shell = alloc_shell(15, 0, 9);
    evaluate(shell, cmd_line);
    // should print:
    // evaluate line:   ls -la   /mpcs51082-aut23 
    // argv[0]=ls 
    // argv[1]=-la 
    // argv[2]=/mpcs51082-aut23 
    // argc=3
}