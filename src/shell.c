#include "shell.h"
#include "job.h"
#include "history.h"
#include "signal_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

// If any of these parameters are equal to 0 in alloc_shell
// then I will use the following default values for the corresponding limits:
const int MAX_LINE = 1024;
const int MAX_JOBS = 16;
const int MAX_HISTORY = 10;

extern char **environ;



// hepler functions:
// helper function to check if waitfg can break
bool check_pid_finish(pid_t pid, msh_t *shell);
// helper function to check if all the jobs has finished
bool check_all_finish(msh_t *shell);
// helper function to wait foreground job to finish
void waitfg(pid_t pid, msh_t *shell);
// helper function to decide whether a job passed in evaluate function is a built in one
bool check_is_builtin(char **argv, int argc);
// helper function to switch case and call execute built in methods
char *builtin_cmd(msh_t *shell, char **argv);
// helper functions to check if a given command is "!N"
bool check_is_n(char *argv);

// built in execution functions:
// 1. jobs
void builtin_cmd_jobs(msh_t *shell, char **argv);
// 2. history - uses the provided print_history function
// 3. !N
void builtin_cmd_n(msh_t *shell, char **argv);
// 4. bg <job>
void builtin_cmd_bg(msh_t *shell, char **argv);
// 5. fg<job>
void builtin_cmd_fg(msh_t *shell, char **argv);
// 6. kill SIG_NUM PID
void builtin_cmd_kill(msh_t *shell, char **argv);

// helper function to check if waitfg can break
// 2 situation can break: get deleted from jobs array; suspended
bool check_pid_finish(pid_t pid, msh_t *shell){
    if(!shell) return false;
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index] && shell->jobs[index]->pid==pid){
           if(shell->jobs[index]->state==SUSPENDED){
            return true;
           }else{
            return false;
           }
        }
    }
    // not find, indicate this pid was deleted
    return true;
}

// helper function to check if all the jobs has finished
bool check_all_finish(msh_t *shell){
    if(!shell) return false;
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index]!=NULL && (shell->jobs[index]->state==BACKGROUND || shell->jobs[index]->state==FOREGROUND)){
        //    printf("in exit-check_all_finish: pid=%d, cmd=%s is still running.", 
        //    shell->jobs[index]->pid, shell->jobs[index]->cmd_line);
           return false;
        }
    }
    return true;
}


// helper function to wait foreground job to finish
void waitfg(pid_t pid, msh_t *shell){
    while(1){
        // [ask TA] should I loop over the job list to see if the foreground job was deleted? 
        // if the specific freground job deleted or suspended, then break
        if(check_pid_finish(pid, shell)) break;
        sleep(1);
    }
}



// helper function to decide whether a job passed in evaluate function is a built in one
/*
char **argv, int argc is from separate_args
*/
bool check_is_builtin(char **argv, int argc){

    // case 1: jobs
    if(argc==1 && strcmp(argv[0], "jobs")==0){
        return true;
    }else if(argc==1 && strcmp(argv[0], "history")==0){
        return true;
    }else if(argc==1 && check_is_n(argv[0])){
        // printf("in check_is_builtin: this is a !N command\n");
        return true;
    }else if (argc==2 && strcmp(argv[0], "bg")==0){
        return true;
    }else if (argc==2 && strcmp(argv[0], "fg")==0){
        return true;
    }else if(argc==3 && strcmp(argv[0], "kill")==0){
        return true;
    }
    return false;
}

bool check_is_n(char *argv){
    if(!argv || argv[0]!='!' || strlen(argv)<=1) return false;
    int i=1;
    for( ; argv[i]!='\0'; ++i){
        if(argv[i]<'0' || argv[i]>'9') {
            // printf("in check_is_n argv[%d]: %c\n", i, argv[i]);
            return false;}
    }
    // printf("in check_is_n: true\n");
    return true;
}

// helper function to switch case and call execute built in methods
char *builtin_cmd(msh_t *shell, char **argv){

    if(strcmp(argv[0], "jobs")==0){
        builtin_cmd_jobs(shell, argv);
    }else if(strcmp(argv[0], "history")==0){
        print_history(shell->histories);
    }else if(argv[0][0]=='!'){
        // printf("in builtin_cmd: find !\n");
        builtin_cmd_n(shell, argv);
    }else if(strcmp(argv[0], "bg")==0){
        builtin_cmd_bg(shell, argv);
    }else if(strcmp(argv[0], "fg")==0){
        builtin_cmd_fg(shell, argv);
    }else if(strcmp(argv[0], "kill")==0){
        builtin_cmd_kill(shell, argv);
    }
    return NULL;
}

void builtin_cmd_jobs(msh_t *shell, char **argv){
    // printf("in builtin_cmd_jobs: jobs\n");
        int index = 0;
        for( ; index<shell->max_jobs; ++index){
            if(shell->jobs[index]){
                printf("[%d]\t%d\t%s\t%s\n",
                shell->jobs[index]->jid,
                shell->jobs[index]->pid,
                (shell->jobs[index]->state<=1)?("RUNNNING"):("SUSPENDED"),
                shell->jobs[index]->cmd_line
                );
            }
        }
}

void builtin_cmd_n(msh_t *shell, char **argv){
    if(strlen(argv[0])<=1) return;
    int history_id = atoi(argv[0] + 1)-1;
    // find the target command line from history    
    if(!(history_id <= shell->max_history && shell->histories->lines[history_id])){
        return ;
    }

    int type;
    char * cmd_line = malloc(strlen(shell->histories->lines[history_id]) + 1);
    
    strcpy(cmd_line, shell->histories->lines[history_id]);
    add_line_history(shell->histories, cmd_line);

    char *cur_job = parse_tok(cmd_line, &type);

    // Process the first token and check for exit condition
    evaluate(shell, cur_job, type);

    // Process subsequent tokens if any
    while (cur_job = parse_tok(NULL, &type)){
        evaluate(shell, cur_job, type);
    };

    // Free the command line buffer at the end of each loop iteration
    free(cmd_line);
    cmd_line = NULL;
    
}


// put a SUSPENDED job in background
void builtin_cmd_bg(msh_t *shell, char **argv){

    // convert the string eg "%71" to int 71
    pid_t pid;
    if(argv[1][0]!='%'){
        pid = atoi(argv[1]);
        // printf("in builtin_cmd_bg: pid = %d\n", pid);
    }else{
        int jid = atoi(argv[1]+1); 
        if(jid>=0 && jid<=shell->max_jobs && shell->jobs[jid]){
            pid = shell->jobs[jid]->pid;
        }
    }
    // loops the job array to find the pid of suspended job
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index] && shell->jobs[index]->state==SUSPENDED && shell->jobs[index]->pid==pid){
            // printf("in builtin_cmd, sent signal to pid=%d:\n", shell->jobs[index]->pid);
            shell->jobs[index]->state = BACKGROUND;
            kill(-shell->jobs[index]->pid, SIGCONT);
        }
    }        
}


// put a SUSPENDED or BACKGROUNG job in foreground
void builtin_cmd_fg(msh_t *shell, char **argv){
    // convert the string eg "%71" to int 71
    pid_t pid;
    if(argv[1][0]!='%'){
        pid = atoi(argv[1]);
    }else{
        int jid = atoi(argv[1]+1); 
        if(jid>=0 && jid<=shell->max_jobs && shell->jobs[jid]){
            pid = shell->jobs[jid]->pid;
        }
    }
    // loops the job array to find the pid of the suspended job
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index] && (shell->jobs[index]->state==SUSPENDED || shell->jobs[index]->state==BACKGROUND) && shell->jobs[index]->pid==pid){
            // printf("in builtin_cmd, sent signal to pid=%d:\n", shell->jobs[index]->pid);
            shell->jobs[index]->state = FOREGROUND;
            kill(-shell->jobs[index]->pid, SIGCONT);
            // wait fg to complete
            waitfg(pid, shell);
        }
    }
}

void builtin_cmd_kill(msh_t *shell, char **argv){
    int sig_num = atoi(argv[1]);
    pid_t pid;
    if(argv[2][0]!='%'){
        pid = atoi(argv[2]);
    }else{
        int jid = atoi(argv[2]+1); 
        if(jid>=0 && jid<=shell->max_jobs && shell->jobs[jid]){
            pid = shell->jobs[jid]->pid;
        }
    }
    
    // printf("in builtin_cmd_kill: sig_num= %d , pid = %d\n", sig_num, pid);
    if(sig_num==2 || sig_num==9){
        // SIG_INT or SIGKILL [ask TA: should they be the same]
        // loops the job array to find the job with this pid
        int index = 0;
        for( ; index<shell->max_jobs; ++index){
            if(shell->jobs[index] && shell->jobs[index]->pid==pid && shell->jobs[index]->pid != 0){
                kill(-shell->jobs[index]->pid, SIGINT);
                // printf("in builtin_cmd_kill SIGINT: 02 stoped a job pid=%d, index=%d\n", shell->jobs[index]->pid, index);
            }
        }
    }else if(sig_num==18){
        // SIGCONT
        int index = 0;
        for( ; index<shell->max_jobs; ++index){
            if(shell->jobs[index] && shell->jobs[index]->pid==pid && shell->jobs[index]->state==SUSPENDED){
                shell->jobs[index]->state = BACKGROUND;
                kill(-shell->jobs[index]->pid, SIGCONT);
            }
        }
    }else if(sig_num==19){
        // SIGSTP
        int index = 0;
        for( ; index<shell->max_jobs; ++index){
            if(shell->jobs[index] && shell->jobs[index]->pid==pid){
                if (shell->jobs[index]->pid != 0) {
                    // update the state of job to SUSPEND
                    shell->jobs[index]->state = SUSPENDED;
                    // Send the SIGINT signal to the foreground process group
                    kill(-shell->jobs[index]->pid, SIGTSTP);
                    // printf("in builtin_cmd_kill SIGSTP: 19 suspended a job pid=%d, index=%d\n", shell->jobs[index]->pid, index);
                }
            }
        }
    }else{
        printf("error: invalid signal number. \n");
    }


}



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
    // load histories
    new_shell->histories = alloc_history(new_shell->max_history);

    // setup the signal handlers
    initialize_signal_handlers();

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

    // check here if the job is built in
    *is_builtin = check_is_builtin(argv, *argc);

    return argv;
}

int evaluate(msh_t *shell, char *line, int job_type){
    if(!line) return 0;
    // check if the line surpasses the maximum number of characters
    if(strlen(line)>shell->max_line){
        printf("error: reached the maximum line limit\n");
        return 0;
    }
    
    char **argv;
    int argc;
    bool is_builtin_temp;
    // parse the line using separate_args
    argv = separate_args(line, &argc, &is_builtin_temp);

    if(argc==0){
        return 0;
    }

    // check if it needs to exit
    if(argc==1 && strcmp("exit", argv[0])==0){
        
        return 1;
    }

    // check if use built_in
    if(is_builtin_temp){
        builtin_cmd(shell, argv);
        return 0;
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

    sigset_t mask_all, mask_one, prev_one;
    sigfillset(&mask_all);
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);
    /* Block SIGCHLD before I fork*/
    sigprocmask(SIG_BLOCK, &mask_one, &prev_one); 

    int child_status;
    pid_t pid = fork();

    // Handle fork() error
    if (pid == -1) {
        perror("fork failed");
        return 0;
    }

    // 1. Create a new child process using fork()
    if (pid  == 0){
        // child running: built in / not built in

        // UNBLOCK ALL
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
        setpgid(0, 0);
        // 3. The child process will then call execve(...) to execute the job.
        if (execve(argv[0], argv, environ) < 0) {
            printf("%s: Command not found.\n", argv[0]);
            exit(1);
        }
        // if(is_builtin_temp){
        //     builtin_cmd(shell, argv);
        // }else{
            
        // }

       

    }else{
        // parent running
        
        // block all signal, so that I will not be interrupted when I am 
        // adding the job to job list
        sigprocmask(SIG_BLOCK, &mask_all, NULL);

        // 2. Have the parent add the newly created job to the jobs array
        add_job(shell->jobs, shell->max_jobs, pid, job_type, line);

        // UNBLOCK ALL
        sigprocmask(SIG_SETMASK, &prev_one, NULL);

        // 4. The parent process will block using waitpid, until the child process end
        if(job_type == FOREGROUND){

            waitfg(pid, shell);
            
        }
        
    }

    return 0;
}

void exit_shell(msh_t *shell){
    if(shell){

        // kill all suspended jobs
        // int index = 0;
        // for( ; index<shell->max_jobs; ++index){
        //     if(shell->jobs[index] && shell->jobs[index]->state==SUSPENDED){
        //         kill(-shell->jobs[index]->pid, SIGINT);
        //     }
        // }
        
        // check if all the jobs has completed, then exit
        // similar to waitfg
        while(1){
            
            // if all the job finshed (a.k.a deleted in jobs array), then exit
            if(check_all_finish(shell)){
                break;
            };
            sleep(1);
        }

        if(shell->jobs) free_jobs(shell->jobs, shell->max_jobs);
        free_history(shell->histories);
        free(shell);
        shell = NULL;
    }
}


