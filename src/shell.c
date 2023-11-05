#include "shell.h"



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

    // to be implemented
    return NULL;
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

    static char* buffer;
    if(line){
        buffer = line;
    }

    
    if(!buffer){
        job_type = (int*) malloc (sizeof(int));
        job_type = -1;
        return NULL;
    }



    
    return line;

}