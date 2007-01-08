CXX = g++ 

PATH_TO_CORE=../../exp_core
SDL_INC = /usr/include/SDL

INC = -I $(PATH_TO_CORE)/src/engine/ -I $(SDL_INC) \
-I $(PATH_TO_CORE)/src/guichan_addon/ 

FLAGS= $(INC)

LIBS = -L $(PATH_TO_CORE)/src/engine/ -lfife -lSDL -lGL -lGLU -llua50 -llualib50 -lSDL_image -lSDL_ttf \
-L $(PATH_TO_CORE)/src/guichan_addon/ -lguichanfife -lguichan \
-lguichan_sdl -lguichan_opengl

frminfo: main.cpp png_write.cpp
	$(CXX) $(FLAGS) -o frminfo main.cpp png_write.cpp $(LIBS)

clean:
	rm -f frminfo
