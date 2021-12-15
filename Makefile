CC = gcc
CFLAGS = 
LFLAGS = 
PROJECT = console-chess
OBJS = main.o
DEPS = main.c chess_utils.c

all: $(DEPS)
	$(CC) $(DEPS) -o $(PROJECT)

clean:
	rm *.o $(PROJECT)

