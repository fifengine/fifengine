CFLAGS = -Werror -Wall -Wno-unused

all: allegro opengl sdl

allegro:
	g++ allegrohelloworld.cpp -o allegrohelloworld $(CFLAGS) \
	-lguichan_allegro -lguichan `allegro-config --libs --cflags`
	g++ allegrowidgets.cpp -o allegrowidgets  $(CFLAGS) \
	-lguichan_allegro -lguichan `allegro-config --libs --cflags`
	g++ allegroaction.cpp -o allegroaction $(CFLAGS) \
	-lguichan_allegro -lguichan `allegro-config --libs --cflags`

opengl:
	g++ openglhelloworld.cpp -o openglhelloworld $(CFLAGS) \
	-lguichan_sdl -lguichan_opengl -lguichan -lSDL_image `sdl-config --libs --cflags` -lGL
	g++ openglwidgets.cpp -o openglwidgets $(CFLAGS) \
	-lguichan_sdl -lguichan_opengl -lguichan -lSDL_image `sdl-config --libs --cflags` -lGL

sdl:
	g++ sdlhelloworld.cpp -o sdlhelloworld $(CFLAGS) \
	-lguichan_sdl -lguichan -lSDL_image `sdl-config --libs --cflags`
	g++ sdlwidgets.cpp -o sdlwidgets $(CFLAGS) \
	-lguichan_sdl -lguichan -lSDL_image `sdl-config --libs --cflags`
	g++ sdlaction.cpp -o sdlaction $(CFLAGS) \
	-lguichan_sdl -lguichan -lSDL_image `sdl-config --libs --cflags`

clean:
	rm -f allegrohelloworld allegrowidgets allegroaction
	rm -f sdlhelloworld sdlwidgets sdlaction
	rm -f openglhelloworld openglwidgets