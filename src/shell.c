#include "shell.h"
#include "job.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// If any of these parameters are equal to 0 in alloc_shell
// then I will use the following default values for the corresponding limits:
const int MAX_LINE = 1024;
const int MAX_JOBS = 16;
const int MAX_HISTORY = 10;

extern char **environ;



msh_t *alloc_shell(int max_jobs, int max_line, int max_history){
    msh_t* new_shell = (msh_t*) malloc (sizeof(msh_t));
    // use default value if necessary
    new_shell->max_jobs= ((max_jobs==0)?(MAX_JOBS):(max_jobs));
    new_shell->max_line= ((max_line==0)?(MAX_LINE):(max_line));
    new_shell->max_history= ((max_history==0)?(MAX_HISTORY):(max_history));
    // allocate job pool and initialize all the positions to be null
    new_shell->jobs = (job_t**) malloc (sizeof(job_t*)*new_shell->max_jobs);
    if (new_shell->jobs != NULL) {
        for (int i = 0; i < new_shell->max_jobs; i++) {
            new_shell->jobs[i] = NULL;
        }
    }
    return new_shell;
}


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

    // remember the first character that the job starts
    char* start = buffer;

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

    // if there's no valid argument
    if(*argc==0) return NULL;

    char** argv = (char**) malloc (sizeof (char *)*(*argc+1));
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
                argv[cnt] = (char *) malloc(sizeof (char )*(end-start+1));
                memcpy(argv[cnt], line+start, end-start);
                argv[cnt][end-start] = '\0';
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

int evaluate(msh_t *shell, char *line, int job_type){
    if(!line) return 0;
    // check if the line surpasses the maximum number of characters
    if(strlen(line)>shell->max_line){
        printf("error: reached the maximum line limit\n");
        return 0;
    }
    // assume is_builtin_temp is true, this argument will be developed in the future
    bool is_builtin_temp = true;
    char **argv;
    int argc;
    // parse the line using separate_args
    argv = separate_args(line, &argc, &is_builtin_temp);

    if(argc==0){
        return 0;
    }

    // check if it needs to exit
    if(argc==1 && strcmp("exit", argv[0])==0){
        free(line);
        line = NULL;
        exit_shell(shell);
        return 1;
    }

    //out put the result
    int i=0;
    // for( ; i<argc; ++i){
    //     printf("argv[%d]=%s\n", i, argv[i]);
    // }
    // printf("argc=%d\n", argc);


    // 5 steps to execute the line

    // [ask TA] force check? what if the child process gets executed first, will it
    // escape the checking in parent?

    // 0. check if there's any free space in job array
    if(!check_free_pos(shell->jobs, shell->max_jobs)){
        printf("The job array is full. Unable to add new jobs.\n");
        return 0;
    }

    int child_status;
    pid_t pid = fork();

    // Handle fork() error
    if (pid == -1) {
        perror("fork failed");
        return 0;
    }

    // 1. Create a new child process using fork()
    if (pid  == 0){
        // child running
        // 3. The child process will then call execve(...) to execute the job.
        if (execve(argv[0], argv, environ) < 0) {
            printf("%s: Command not found.\n", argv[0]);
            exit(1);
        }

    }else{
        // parent running
        // 2. Have the parent add the newly created job to the jobs array
        add_job(shell->jobs, shell->max_jobs, pid, job_type, line);

        // 4. The parent process will block using waitpid, until the child process end
        if(job_type == FOREGROUND){
            pid_t wpid = waitpid(pid, &child_status, 0);

            if (WIFEXITED(child_status)){
                // 5. the child process terminates, the parent process must delete the job from the jobs array.
                delete_job(shell->jobs, shell->max_jobs, pid);
            }else{
                printf("Child %d terminate abnormally\n", wpid);
                // [ask TA] is there any ca the case that the child does not end normally?
                // should I delete the child then?
            }
        }else if(job_type==BACKGROUND){
            pid_t term_pid = waitpid (pid, &child_status, WNOHANG);

            // not sure whether I should do sth here, can I delete the completed job here?
            if (term_pid > 0) {
                // The job has completed
                delete_job(shell->jobs, shell->max_jobs, shell->jobs[index]->pid);
            }
        }
    }

    // [ask TA] should I move this to the front of evaluate function?
    // Since evaluate function has multiple exits

    // reaps any completed background jobs
    // traverse the shell job array, if it is a background job, check if it has completed
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index] && shell->jobs[index]->state==BACKGROUND){
            pid_t term_pid = waitpid(shell->jobs[index]->pid, &child_status, WNOHANG);
            if (term_pid > 0) {
                // The job has completed
                delete_job(shell->jobs, shell->max_jobs, shell->jobs[index]->pid);
            }
        }
    }

    return 0;
}

void exit_shell(msh_t *shell){
    if(shell){
        if(shell->jobs) free_jobs(shell->jobs, shell->max_jobs);
        free(shell);
        shell = NULL;
    }
}