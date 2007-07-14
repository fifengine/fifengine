SUBDIRS = widgets @GUICHAN_EXTRADIRS@

INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libguichan.la

libguichan_la_LIBADD = widgets/libguichan_widgets.la

libguichan_la_DEPENDENCIES = \
	widgets/libguichan_widgets.la

# The Gui-chan library sources
libguichan_la_SOURCES = 	\
	actionevent.cpp		\
	basiccontainer.cpp	\
	cliprectangle.cpp	\
	color.cpp		\
	defaultfont.cpp		\
	event.cpp		\
	exception.cpp		\
	focushandler.cpp	\
	font.cpp		\
	graphics.cpp		\
	gui.cpp			\
	guichan.cpp		\
	image.cpp		\
	imagefont.cpp		\
	inputevent.cpp		\
	key.cpp			\
	keyevent.cpp		\
	keyinput.cpp		\
	mouseevent.cpp		\
	mouseinput.cpp		\
	rectangle.cpp		\
	widget.cpp
