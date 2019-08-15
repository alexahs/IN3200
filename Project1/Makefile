CC 			:= gcc
FLAGS		:= -Wall -g
LIBS 		:= -lm
EXEC 		:= pageRank.exe
DEP 		:= PE_functions_15222.h
SRC 		:= $(wildcard *.c)


.PHONY : clean

# default: $(EXEC)
# all: default


OBJECTS		:= $(patsubst *.c, *.o, $(wildcard *.c))
HEADERS		:= $(wildcard *.h)


%.o: %.c $(HEADERS)
	$(CC) $(FLAGS) -fopenmp -c $< $@

$(EXEC): $(OBJECTS)
	@$(CC) $(OBJECTS) -fopenmp $(LIBS) -o $@
	@echo $(EXEC) ready!


clean :
			@rm -f *.o $(EXEC)
			@echo "Clean-up complete!"
