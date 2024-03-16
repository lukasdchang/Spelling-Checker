CC = gcc
CFLAGS = -I.
DEPS =  # Add your header files here if needed
OBJ = spchk.o

all: spchk

spchk: $(OBJ)
    $(CC) -o $@ $^ $(CFLAGS)

spchk.o: spchk.c $(DEPS)
    $(CC) -c -o $@ $< $(CFLAGS)

clean:
    rm -f *.o spchk