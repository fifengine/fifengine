CC=cc
CFLAGS=-O
LDFLAGS=--shared -lz
prefix=/usr
libdir=$(prefix)/lib

ZIP_OBJS = unzip.o ioapi.o

.c.o:
	$(CC) -c $(CFLAGS) $*.c

all: libminizip

libminizip:  $(ZIP_OBJS)
	$(CC) $(LDFLAGS) -o libminizip.so $(ZIP_OBJS)

clean:
	/bin/rm -f *.o *~ libminizip.so

install:
	mkdir -p $(prefix)/lib
	cp libminizip.so $(prefix)/lib/libminizip.so
