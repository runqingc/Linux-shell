#include "signal_handlers.h"
#include "job.h"
#include "shell.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>

// gloable variable
// the shell I used in main
extern msh_t* shell;

/*
* sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
*     a child job terminates (becomes a zombie), or stops because it
*     received a SIGSTOP or SIGTSTP signal. The handler reaps all
*     available zombie children, but doesn't wait for any other
*     currently running children to terminate.
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
void sigchld_handler(int sig)
{
    // printf("in sigchld_handler\n");
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;
    sigfillset(&mask_all);
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) { /* Reap child */
        // printf("in sigchld_handler: pid=%d\n", pid);
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        /* Delete the child from the job list if it has terminated*/
        if (WIFEXITED(status) || WIFSIGNALED(status)){
            delete_job(shell->jobs, shell->max_jobs, pid);
            // printf("in sigchld_handler deleted: pid=%d\n", pid);
        }
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    if (errno != ECHILD){
        // is this error function correct? "waitpid error"
        strerror(1);
    }
    errno = olderrno;
}

/*
* sigint_handler - The kernel sends a SIGINT to the shell whenver the
*    user types ctrl-c at the keyboard.  Catch it and send it along
*    to the foreground job.
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
void sigint_handler(int sig)
{
    
    // loops the job array to find the pid of foreground job
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index] && shell->jobs[index]->state==FOREGROUND){
            if (shell->jobs[index]->pid != 0) {
                // printf("in sigint_handler: stoped a job pid=%d\n", shell->jobs[index]->pid);
                // Send the SIGINT signal to the foreground process group
                kill(-shell->jobs[index]->pid, SIGINT);
                
            }
        }
    }

}

/*
* sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
*     the user types ctrl-z at the keyboard. Catch it and suspend the
*     foreground job by sending it a SIGTSTP.
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
void sigtstp_handler(int sig)
{   
    // block SIGCHLD so that shell->jobs[index]->state = SUSPENDED; will be executed first
    sigset_t mask_one, prev_one;
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask_one, &prev_one); 
    // loops the job array to find the pid of foreground job
    int index = 0;
    for( ; index<shell->max_jobs; ++index){
        if(shell->jobs[index] && shell->jobs[index]->state==FOREGROUND){
            if (shell->jobs[index]->pid != 0) {
                // printf("in sigtstp_handler: suspended a job pid=%d\n", shell->jobs[index]->pid);
                // update the state of foreground job to SUSPEND
                shell->jobs[index]->state = SUSPENDED;
                // Send the SIGINT signal to the foreground process group
                kill(-shell->jobs[index]->pid, SIGTSTP);
                // printf("in sigtstp_handler: 02 suspended a job pid=%d, index=%d\n", shell->jobs[index]->pid, index);
            }
        }
    }
    // UNBLOCK ALL
    sigprocmask(SIG_SETMASK, &prev_one, NULL);
}

/*
* setup_handler - wrapper for the sigaction function
*
* Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
*/
typedef void handler_t(int);
handler_t *setup_handler(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0) {
        perror("Signal error");
        exit(1);
    }
    return (old_action.sa_handler);
}


void initialize_signal_handlers() {

    // sigint handler: Catches SIGINT (ctrl-c) signals.
    setup_handler(SIGINT,  sigint_handler);   /* ctrl-c */
    // sigtstp handler: Catches SIGTSTP (ctrl-z) signals.
    setup_handler(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    // sigchld handler: Catches SIGCHILD signals.
    setup_handler(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
}