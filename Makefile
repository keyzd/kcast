CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lSDL2 -lm

PROGNAME = kcast
SRCMODULES = main.c kc_renderer.c kc_debug.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

kcast: $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

run: $(PROGNAME)
	./kcast 512 512 0

clean:
	rm -f *.o $(PROGNAME)
