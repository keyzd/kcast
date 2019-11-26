CC = gcc
CFLAGS = -g
LDFLAGS = -lSDL2 -lSDL2_image -lm

PROGNAME = kcast
SRCMODULES = main.c renderer.c debug.c misc.c math.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

kcast: $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

run: $(PROGNAME)
	./kcast 512 512 0

clean:
	rm -f *.o $(PROGNAME)
