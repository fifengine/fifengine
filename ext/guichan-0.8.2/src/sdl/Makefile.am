#noinst_LTLIBRARIES = libsdl.la

lib_LTLIBRARIES = libguichan_sdl.la

INCLUDES = -I$(top_srcdir)/include

libguichan_sdl_la_LDFLAGS = -no-undefined -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

libguichan_sdl_la_SOURCES =	\
	sdl.cpp			\
	sdlgraphics.cpp		\
	sdlimage.cpp		\
	sdlimageloader.cpp	\
	sdlinput.cpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = guichan_sdl-0.8.pc

EXTRA_DIST = guichan_sdl-0.8.pc.in
