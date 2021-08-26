CC=clang
CFLAGS=-Wall -Wpedantic -ggdb `pkg-config --cflags sdl2 glew`
LDLIBS=`pkg-config --libs sdl2 glew`

main: main.c canvas.c matrix.c
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}
