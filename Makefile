CC=gcc

CFLAGS=-Wall
LDFLAGS=

OBJS=backlight.o

PREFIX=/usr/local/bin

.PHONY: clean install uninstall default
default: backlight

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

backlight: $(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) 

clean:
		rm -f $(OBJS)
		rm -f backlight

install:
		install --mode=4755 backlight $(PREFIX)
		ln --force --symbolic $(PREFIX)/backlight $(PREFIX)/backlight_up
		ln --force --symbolic $(PREFIX)/backlight $(PREFIX)/backlight_down

uninstall:
		rm --force $(PREFIX)/backlight_up
		rm --force $(PREFIX)/backlight_down
		rm --force $(PREFIX)/backlight
