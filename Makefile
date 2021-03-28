CC = gcc
SLC = glslc
CFLAGS = -std=gnu17 -march=native -mtune=native -flto -O2 -Wall -Wextra
SLFLAGS = -O

all: zero

zero: main.o graphics.o network.o window.o content.o shaders/shader.vert.spv shaders/shader.frag.spv
	$(CC) main.o graphics.o network.o window.o content.o -o zero $(CFLAGS)

main.o: main.c main.h.gch
	$(CC) -c main.c -o main.o $(CFLAGS)

main.h.gch: main.h
	$(CC) -c main.h -o main.h.gch $(CFLAGS)

graphics.o: graphics.c graphics.h.gch
	$(CC) -c graphics.c -o graphics.o -lm -lvulkan $(CFLAGS)

graphics.h.gch: graphics.h
	$(CC) -c graphics.h -o graphics.h.gch $(CFLAGS)

network.o: network.c network.h.gch
	$(CC) -c network.c -o network.o -luuid -lpthread $(CFLAGS)

network.h.gch: network.h
	$(CC) -c network.h -o network.h.gch $(CFLAGS)

window.o: window.c window.h.gch
	$(CC) -c window.c -o window.o -lm -lvulkan -lglfw $(CFLAGS)

window.h.gch: window.h
	$(CC) -c window.h -o window.h.gch $(CFLAGS)

content.o: content.c content.h.gch
	$(CC) -c content.c -o content.o -lm $(CFLAGS)

content.h.gch: content.h
	$(CC) -c content.h -o content.h.gch $(CFLAGS)

shaders/shader.vert.spv: shaders/shader.vert
	$(SLC) shaders/shader.vert -o shaders/shader.vert.spv $(SLFLAGS)

shaders/shader.frag.spv: shaders/shader.frag
	$(SLC) shaders/shader.frag -o shaders/shader.frag.spv $(SLFLAGS)

clean:
	rm shaders/*.spv *.gch *.o zero
