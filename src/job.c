#include "../include/job.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "job.h"


bool add_job(job_t **jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line){

    // traverse the job array **job, try to find if there are any null position
    // it now takes O(N) time because I don't have hash table in C
    int index = 0;
    for( ; index<max_jobs; ++index){
        // find an index
        if(jobs[index]==NULL){
            break;
        }
    }
    // cannot find an empty position
    if(index>=max_jobs){
        printf("The job array is full. Unable to add new jobs.\n");
        return false;
    }

    // construct job
    job_t* new_job = (job_t*) malloc(sizeof (job_t));
    new_job->pid = pid;
    new_job->state = state;

    // do not sure what this field is used for
    new_job->jid = index; // set to the index in the job array

    // use deep copy, allocate space first
    new_job->cmd_line = (char *) malloc(sizeof (char)*(strlen(cmd_line)+1));
    strcpy(new_job->cmd_line, cmd_line);

    // add new job to the job list
    jobs[index] = new_job;

    // successfully add a job
    return true;
}

bool delete_job(job_t **jobs, int max_jobs, pid_t pid){

    if(!jobs)return false;

    // traverse the job array to find the job with given pid
    int index = 0;
    for( ; index<max_jobs; ++index){
        if(jobs[index] && jobs[index]->pid==pid){
            if(jobs[index]->cmd_line) free(jobs[index]->cmd_line);
            free(jobs[index]);
            jobs[index] = NULL;
            // successfully deleted a job
            return true;
        }
    }

    // the given job was not found, fail to delete
    printf("The job with pid=%d was not found. Deletion failed. \n", pid);
    return false;
}


void free_jobs(job_t **jobs, int max_jobs){
    if(jobs){
        // free each job first
        int index = 0;
        for( ;index<max_jobs; ++index){
            if(jobs[index]) free(jobs[index]);
            jobs[index] = NULL;
        }
        free(jobs);
    }
}
