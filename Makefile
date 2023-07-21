CLFAGS = -lm -Wall -O2

bin=pnmprt

PREFIX = /usr/local
CC = cc

all: bin/$(bin)

bin/$(bin): src/main.c;
	mkdir -p bin
	$(CC) $^ $(CLFAGS) -o $@

.PHONY: clean install uninstall

clean:
	rm bin/$(bin)


install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp \
		"bin/$(bin)" \
		$(DESTDIR)$(PREFIX)/bin 
	chmod 755 \
		"$(DESTDIR)$(PREFIX)/bin/$(bin)" 

uninstall:
	rm -f \
		"$(DESTDIR)$(PREFIX)/bin/$(bin)" 

