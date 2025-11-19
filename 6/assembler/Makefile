CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = Assembler
OBJS = main.o parser.o coder.o symboltable.o

all: $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c parser.h coder.h symboltable.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

coder.o: coder.c coder.h
	$(CC) $(CFLAGS) -c coder.c

symboltable.o: symboltable.c symboltable.h
	$(CC) $(CFLAGS) -c symboltable.c

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: all clean
