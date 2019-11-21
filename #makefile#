LDFLAGS=-L.
LDLIBS=-lutils
CFLAGS+=-std=c99 -D_POSIX_SOURCE

libutils.a: utils.o
	ar r $@ $?

gcc client.c utils.o -o client -std=gnu99

gcc serveur.c utils.o -o serveur -std=gnu99
