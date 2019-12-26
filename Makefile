CC = gcc
CFLAGS = -g -Wall -pedantic -Wextra
LDFLAGS = -lSDL2 -lSDL2_image -lm

PROGNAME = kcast
SRCMODULES = main.c renderer.c misc.c loader.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

kcast: $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

run: $(PROGNAME)
	./kcast 512 512 0

clean:
	rm -f *.o $(PROGNAME)

