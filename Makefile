CC=gcc
CFLAGS=-I. -g -Wall
OBJ := $(patsubst %.c,%.o,$(wildcard *.c))
TARGET = Clox

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CFLAGS)

.PHONY: clean
clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
