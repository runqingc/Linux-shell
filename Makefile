CC=gcc
CFLAGS=-I./include
SRC_DIR=./src
OBJ_DIR=./bin
DEPS=./include/job.h ./include/shell.h ./include/history.h ./include/signal_handlers.h
_OBJ = msh.o job.o shell.o history.o signal_handlers.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))
EXECUTABLE=msh

# This rule creates object files from c source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# This rule links object files into the final executable
$(EXECUTABLE): $(OBJ)
	$(CC) -o $(OBJ_DIR)/$@ $^ $(CFLAGS)

.PHONY: clean run

# This rule is for cleaning up the build (removing object files and the executable)
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCDIR)/*~ 

# This rule is for running the executable
run: $(EXECUTABLE)
	./$(OBJ_DIR)/$(EXECUTABLE)
