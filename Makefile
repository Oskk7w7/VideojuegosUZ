all: build
	g++ -I src/include \
	    -L src/lib \
	    -o build/main src/main.cpp \
	    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

build:
	if not exist build mkdir build