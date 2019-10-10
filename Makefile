CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lSDL2 -lm

PROGNAME = kcast
SRCMODULES = main.c kc_renderer.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

kc_renderer.o: kc_renderer.c kcast.h

kcast: $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

run: $(PROGNAME)
	./kcast 320 240 0

clean:
	rm -f *.o $(PROGNAME)
