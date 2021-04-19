CC = gcc
CFLAGS = -std=gnu17 -march=native -mtune=native -flto -O2 -Wall -Wextra
LDLIBS = -lm -luuid -lpthread -lvulkan -lglfw

SLC = glslc
SLFLAGS = -O

.PHONY: all clean

all: zeroClient shaders/vertex.spv shaders/fragment.spv

zeroClient: zeroClient.o platformBase.o networkSystem.o renderSystem.o contentManager.o gameLogic.o
	$(CC) $^ -o $@ $(LDLIBS) $(CFLAGS)

zeroClient.o: zeroClient.c platformBase.h networkSystem.h renderSystem.h contentManager.h gameLogic.h
	$(CC) -c $< $(CFLAGS)

platformBase.o: platformBase.c platformBase.h
	$(CC) -c $< $(CFLAGS)

networkSystem.o: networkSystem.c networkSystem.h
	$(CC) -c $< $(CFLAGS)

renderSystem.o: renderSystem.c renderSystem.h renderInternal.h
	$(CC) -c $< $(CFLAGS)

contentManager.o: contentManager.c contentManager.h
	$(CC) -c $< $(CFLAGS)

gameLogic.o: gameLogic.c gameLogic.h
	$(CC) -c $< $(CFLAGS)

shaders/vertex.spv: shaders/vertex.vert
	$(SLC) $< -o $@ $(SLFLAGS)

shaders/fragment.spv: shaders/fragment.frag
	$(SLC) $< -o $@ $(SLFLAGS)

clean:
	rm shaders/*.spv *.o zeroClient
