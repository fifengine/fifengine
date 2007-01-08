CXX = g++
INC = -I /usr/include/SDL
CXXFLAGS = $(INC)
LIBS = -lSDL_image -lSDL -lpng

tilemaker: imgtool.cpp usage_tilemaker.cpp
	$(CXX) $(CXXFLAGS) -o tilemaker imgtool.cpp usage_tilemaker.cpp $(LIBS)

usage_tilemaker.cpp: imgtool_usage.txt
	echo '#include <iostream>' >usage_tilemaker.cpp
	echo 'void usage(const char* name) {' >>usage_tilemaker.cpp
	echo 'std::cout << "USAGE: " << name;' >>usage_tilemaker.cpp
	awk -f reformat.awk imgtool_usage.txt >>usage_tilemaker.cpp
	echo '}' >>usage_tilemaker.cpp

clean:
	rm -f tilemaker usage_tilemaker.cpp
