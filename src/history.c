#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *HISTORY_FILE_PATH = "./data/.msh_history";

history_t *alloc_history(int max_history){

    history_t * new_history_t = (history_t *) malloc (sizeof (history_t));
    new_history_t->lines = (char**) malloc (sizeof (char*)*max_history);
    new_history_t->max_history = max_history;
    int i;
    for(i=0; i<max_history; ++i){
        new_history_t->lines[i] = NULL;
    }
    // next points to the next available slot
    new_history_t->next = 0;

    // open file to load history
    FILE *fp = fopen(HISTORY_FILE_PATH, "r");
    if (fp != NULL){
        // printf("in alloc_history: find a history file\n");

        char *line = NULL;
        long int len = 0;
        long nRead = getline(&line, &len, fp);
        // use add_line_history to add all history

        while (nRead != -1 && new_history_t->next<max_history){
            if(line[nRead - 1] == '\n') {
                line[nRead -1] = '\0';
            }

            add_line_history(new_history_t, line);

            free(line);
            line = NULL;
            nRead = getline(&line, &len, fp);
        }
        fclose(fp);
    }
    // else{
    //     printf("in alloc_history: open history file failed\n");
    // }

    return new_history_t;
}


// ask TA: is it low efficient to shift the elements over by one and then insert the new one?
// making it a loop will be better? (e.g. using the array to implement queue)
// here I just used the implementation suggested
void add_line_history(history_t *history, const char *cmd_line){

    // check if the cmd_line is empty
    // here, empty = not null
    // can filter those have no valid character here if future implementation need
    if(cmd_line==NULL) return;
    // printf("in add_line_history: next=%d\n", history->next);
    // check if the history has reached max
    if(history->next==history->max_history){
        // shift the elements over by one
        int i=0;
        for( ; i<history->max_history; ++i){
            // free itself, fill it with next history(as long as it is not the last one)
            free(history->lines[i]);
            history->lines[i] = NULL;
            if(i<history->max_history-1){
                history->lines[i] = (char *) malloc(strlen(history->lines[i+1]) + 1);
                strcpy(history->lines[i], history->lines[i+1]);
            }
        }
        --history->next;
    }
    // printf("in add_line_history: next=%d\n", history->next);
    history->lines[history->next] = (char *) malloc(strlen(cmd_line) + 1);
    strcpy(history->lines[history->next], cmd_line);
    history->next++;
    // printf("in add_line_history: %d %s\n", history->next-1, history->lines[history->next-1]);
}

void print_history(history_t *history) {
    // printf("in print_history: \n");
    for(int i = 1; i <= history->next; i++) {
        printf("%5d\t%s\n",i,history->lines[i-1]);
    }
}

char *find_line_history(history_t *history, int index){
    // the index starts 1, so this is the actual index I want to find
    index--;

    if(index<0 || index>=history->max_history) return NULL;

    return history->lines[index];
}

void free_history(history_t *history){
    FILE *file = fopen(HISTORY_FILE_PATH, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    int i=0;
    for( ; i<history->next; ++i){
        fprintf(file, "%s\n", history->lines[i]);
        free(history->lines[i]);
    }
    // Close the file
    fclose(file);
    // free the history
    free(history);

}
