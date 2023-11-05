CC=gcc
CFLAGS=-I./include
SRC_DIR=./src
OBJ_DIR=./bin
DEPS=./include/shell.h
_OBJ = msh.o shell.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))
EXECUTABLE=msh

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) -o $(OBJ_DIR)/$@ $^ $(CFLAGS)

.PHONY: clean run

clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCDIR)/*~ 

run: $(EXECUTABLE)
	./$(OBJ_DIR)/$(EXECUTABLE)
