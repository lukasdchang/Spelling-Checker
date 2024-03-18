CC=gcc

CFLAGS=

TARGET=spchk

SRC=spchk.c

OBJ=$(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test:
	./$(TARGET) testdict.txt file

run:
	./$(TARGET) $(filter-out $@,$(MAKECMDGOALS))

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean run