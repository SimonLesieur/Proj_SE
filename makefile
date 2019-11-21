LDFLAGS=-L.
LDLIBS=-lutils
CFLAGS+=-std=c99 -D_POSIX_SOURCE

libutils.a: utils.o
	ar r $@ $?
