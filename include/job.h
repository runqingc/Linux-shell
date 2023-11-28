#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef enum job_state{BACKGROUND, FOREGROUND, SUSPENDED, UNDEFINED} job_state_t;

// Represents a job in a shell.
typedef struct job {
    char *cmd_line;     // The command line for this specific job.
    job_state_t state;  // The current state for this job
    pid_t pid;          // The process id for this job
    int jid;            // The job number for this job in the job array
}job_t;

#endif


/*
 * add_job: adds a new job to the array
 *
 * jobs: the target job array that we add job into
 *
 * max_jobs: the max number of jobs that the array can hold
 *
 * pid: the pid of the child shell running this job (?)
 *
 * state: the state of the job, define in enum job_state
 *
 * cmd_line: the command line for this specific job.
 *
 * Returns: true to indicate the job was added, false to indicate the job array has reached its maxium jobs
 */
bool add_job(job_t **jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line);



/*
 * delete_job: delete a job from the job array
 *
 * jobs: the target job array that we delete job from
 *
 * pid: the pid of the job that I want to delete
 *
 * max_jobs: the max number of jobs that the array can hold
 *
 * Returns: true if successfully deleted the job; false otherwise (e.g. if
 * the job pid was not found in the given job array)
 * */

bool delete_job(job_t **jobs, int max_jobs, pid_t pid);


/*
 * free_jobs: deallocate every job, then deallocate the whole job array
 *
 * job_t: the pointer to the job array
 *
 * max_jobs: the max number of jobs that the array can hold
 * */
void free_jobs(job_t **jobs, int max_jobs);

bool check_free_pos(job_t **jobs, int max_jobs);