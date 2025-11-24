CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = VMtranslator
SRCS = main.c vm_tran.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c vm_tran.h
	$(CC) $(CFLAGS) -c main.c

vm_tran.o: vm_tran.c vm_tran.h
	$(CC) $(CFLAGS) -c vm_tran.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean