all: build
	g++ -I src/include \
	    -L src/lib \
	    -o build/Demo2 src/Demo2.cpp \
	    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer

demo1: build
	g++ -I src/include \
	    -L src/lib \
	    -o build/Demo1 src/Demo1.cpp \
	    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer

demo3: build
	g++ -I src/include \
	    -L src/lib \
	    -o build/Demo3 src/Demo3.cpp \
	    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer

build:
	if not exist build mkdir build