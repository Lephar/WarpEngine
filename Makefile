CC = gcc
CFLAGS = -std=gnu17 -march=native -mtune=native -flto -O2 -Wall -Wextra
LDLIBS = -lm -luuid -lpthread -lvulkan -lglfw

SLC = glslc
SLFLAGS = -O

.PHONY: all clean

all: zero

zero: main.o base.o network.o graphics.o content.o game.o shaders/vertex.spv shaders/fragment.spv
	$(CC) main.o base.o network.o graphics.o content.o game.o -o zero $(LDLIBS) $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

base.o: base.c base.h
	$(CC) -c base.c $(CFLAGS)

network.o: network.c network.h
	$(CC) -c network.c $(CFLAGS)

graphics.o: graphics.c graphics.h
	$(CC) -c graphics.c $(CFLAGS)

content.o: content.c content.h
	$(CC) -c content.c $(CFLAGS)

game.o: game.c game.h
	$(CC) -c game.c $(CFLAGS)

shaders/vertex.spv: shaders/vertex.vert
	$(SLC) shaders/vertex.vert -o shaders/vertex.spv $(SLFLAGS)

shaders/fragment.spv: shaders/fragment.frag
	$(SLC) shaders/fragment.frag -o shaders/fragment.spv $(SLFLAGS)

clean:
	rm shaders/*.spv *.o zero
