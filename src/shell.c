#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


// If any of these parameters are equal to 0 in alloc_shell
// then I will use the following default values for the corresponding limits:
const int MAX_LINE = 1024;
const int MAX_JOBS = 16;
const int MAX_HISTORY = 10;


/*
* alloc_shell: allocates and initializes the state of the shell
*
* max_jobs: The maximum number of jobs that can be in existence at any point in time.
*
* max_line: The maximum number of characters that can be entered for any specific command line.
*
* max_history: The maximum number of saved history commands for the shell.
*
* Returns: a msh_t pointer that is allocated and initialized
*/
msh_t *alloc_shell(int max_jobs, int max_line, int max_history){
    msh_t* new_shell = (msh_t*) malloc (sizeof(msh_t));
    // use defualt value if necessary
    new_shell->max_jobs= ((max_jobs==0)?(MAX_JOBS):(max_jobs));
    new_shell->max_line= ((max_line==0)?(MAX_LINE):(max_line));
    new_shell->max_history= ((max_history==0)?(MAX_HISTORY):(max_history));

    return new_shell;
}


/**
* parse_tok: Continuously retrieves separate commands from the provided command line until all commands are parsed
*
* line:  the command line to parse, which may include multiple commands. If line is NULL then parse_tok continues parsing the previous command line.
*
* job_type: Specifies whether the parsed command is a background or foreground job. If no job is returned then assign the value at the address to -1
*
* Returns: NULL no other commands can be parsed; otherwise, it returns a parsed command from the command line.
*
* Please note this function does modify the ``line`` parameter.
*/
char *parse_tok(char *line, int *job_type){

    // static point reuse as means to return back the portions of the string
    static char* buffer;
    
    if(line){
        buffer = line;
    }

    // nothing to parse if buffer is NULL
    if(!buffer){
        if(job_type) *job_type = -1;
        return NULL;
    }

    // skip leading spaces
    while (*buffer == ' ') {
        buffer++;
    }

    // Check if the command line is empty or has no command
    // before the control character
    if (*buffer == '\0' || *buffer == '&' || *buffer == ';') {
        *job_type = -1;
        if(*buffer == '&' || *buffer == ';') ++buffer;
        return NULL;
    }

    // remember the first character that the job starts
    char* start = buffer;

    while(!(*buffer == '\0' || *buffer == '&' || *buffer == ';')){
        ++buffer;
    }

    // Set the job type and terminate the current command
    if (*buffer == '&') {
        *job_type = 0;
        *buffer = '\0';
        buffer++;  // Move past the null terminator for the next call
    } else if (*buffer == ';') {
        *job_type = 1;
        *buffer = '\0';
        buffer++;  // Move past the null terminator for the next call
    } else if (*buffer == '\0') {
        *job_type = 1;  // foreground job if no specifier
    }

    return start;
}


// code reuse from hw4, helper function for separate_args
int compute_num_args(const char *line){
    const char* i = line;
    int cnt=0;
    bool flag = true;
    while(*i){
        if(*i==' '){
            flag = true;
        }else{
            if(flag){
                ++cnt;
                flag = false;
            }
        }
        ++i;
    }
    return cnt;
}


char **separate_args(char *line, int *argc, bool *is_builtin){

    *argc = compute_num_args(line);
    char** argv = (char**) malloc (sizeof (char *)*3);
    int start=-1, end=0, cnt=0;
    while(true){
        if(line[end]!=' ' && line[end]!='\0'){
            // the first valid char of this argument
            if(start==-1){
                start = end;
            }
            ++end;
        }else{
            // first space after a series of valid chars
            if(start>=0){
                argv[cnt] = (char *) malloc(sizeof (char )*(end-start+2));
                memcpy(argv[cnt], line+start, end-start+1);
                argv[cnt][end-start+1] = '\0';
                ++cnt;
                start = -1;
            }
            // if meet the null value
            if(!line[end]) break;
            ++end;
        }
    }
    argv[*argc] = NULL;
    return argv;
}

int evaluate(msh_t *shell, char *line){
    // check if the line surpasses the maximum number of characters
    if(strlen(line)>shell->max_line){
        printf("error: reached the maximum line limit\n");
        // assume 1 is an error code
        return 1;
    }

    // assume is_builtin_temp is true, this argument will be developed in the future
    bool is_builtin_temp = true;
    char **argv;
    int argc;
    // parse the line using separate_args
    argv = separate_args(line, &argc, &is_builtin_temp);
    //out put the result
    int i=0;
    for( ; i<argc; ++i){
        printf("argv[%d]=%s\n", i, argv[i]);
    }
    printf("argc=%d\n", argc);
    // assume it will always return 0 for now, implement in the following homework
    return 0;
}
