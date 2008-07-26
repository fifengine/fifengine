CC=cc -I "." -I "./include/" -I "./OpenAL32/Include"
CFLAGS= -fPIC -D_GNU_SOURCE=1 -Dopenal_EXPORTS -g -O2 -funroll-loops -Wall -Wextra -fno-strict-aliasing -fvisibility=hidden -DHAVE_GCC_VISIBILITY -D_REENTRANT -DAL_BUILD_LIBRARY
LDFLAGS= -shared  -lpthread -ldl -lm -fPIC -g -O2 -funroll-loops
prefix=/usr
libdir=$(prefix)/lib
$srcdir=OpenAL32/*.o
f=""

OPENAL_OBJS = OpenAL32/alBuffer.o OpenAL32/alFilter.o  OpenAL32/alSource.o OpenAL32/OpenAL32.o OpenAL32/alAuxEffectSlot.o OpenAL32/alEffect.o OpenAL32/alError.o OpenAL32/alListener.o OpenAL32/alState.o OpenAL32/alThunk.o OpenAL32/alExtension.o
ALC_OBJS = Alc/ALc.o Alc/ALu.o Alc/alcConfig.o Alc/alcRing.o Alc/alcThread.o Alc/bs2b.o Alc/wave.o Alc/alsa.o Alc/oss.o

#OPENAL_OBJS = OpenAL32/alBuffer.c OpenAL32/alFilter.c
#ALC_OBJS = Alc/*.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

all: openal-soft
	

openal-soft: $(OPENAL_OBJS) $(ALC_OBJS)
	$(CC) $(LDFLAGS) -o libopenal.so *.o
	/bin/rm -f *.o *~

clean:
	/bin/rm -f *.o *~ libopenal.so

install:
	mkdir -p $(prefix)/lib
	cp -f alsoftrc.sample ~/.alsofrc
	cp libopenal.so $(prefix)/lib/libopenal_soft.so
	cp -r -f include/AL $(prefix)/include/
