EXEC 			= serial_main.exe
CC 		    	= gcc
FLAGS       		= -Wall -g -O2
LDFLAGS = -L../simple-jpeg/ -lsimplejpeg

SRC 			:= $(wildcard *.c)

all: simple-jpeg $(EXEC)

simple-jpeg:
	$(MAKE) -C ../simple-jpeg

$(EXEC): $(SRC)
	$(CC) $(FLAGS) $(SRC) -lm -o $@ $(LDFLAGS)
	@echo "Linking complete!"
	@echo $(EXEC) "ready to run!"

clean:
	@rm -rf *.o $(EXEC)
	$(MAKE) -C ../simple-jpeg clean
	@echo "Cleanup complete!"
