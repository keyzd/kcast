CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lSDL2 -lSDL2_image -lm

PROGNAME = kcast
SRCMODULES = main.c kc_renderer.c kc_debug.c kc_misc.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

kcast: $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

run: $(PROGNAME)
	./kcast 800 600 0

clean:
	rm -f *.o $(PROGNAME)
