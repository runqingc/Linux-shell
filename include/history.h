#ifndef _HISTORY_H_
#define _HISTORY_H_

extern const char *HISTORY_FILE_PATH;

//Represents the state of the history of the shell
typedef struct history {
    char **lines;
    int max_history;
    int next;
}history_t;


/*
 * alloc_history: allocating memory for a new history_t value. This
 * function will open the HISTORY_FILE_PATH file and load all prior
 * history into char **lines up until max_history is reached.
 *
 * max_history: the maximum number of saved history commands for the shell.
 *
 * Returns: a history_t pointer that is allocated and initialized
 * */
history_t *alloc_history(int max_history);

/*
 * add_line_history: adds the parameter cmd_line to the history of lines.
 *
 * history: the pointer points to the history_t, where we will add line into
 *
 * cmd_line: the history of lines that we want to add
 * */
void add_line_history(history_t *history, const char *cmd_line);


/*
 * print_history: print out all the history
 *
 * history: the pointer points to the history_t, where we get history records
 * */
void print_history(history_t *history);


/*
 * find_line_history: returns the command line at the specified index.
 * If index is not within the range of [1,max_history] inclusive then the
 * function returns NULL.
 *
 * history: the pointer points to the history_t, where we get history records
 *
 * index: the index of a specific history records we look for
 * */
char *find_line_history(history_t *history, int index);

/*
 * free_history: Save all history inside the char **lines array to the
 * HISTORY_FILE_PATH file, then frees the history_t and all allocated memory associated
 * with the state. 
 * 
 * history: the pointer points to the history_t, where we get history records
 * */
void free_history(history_t *history);

#endif