# C Compiler Options
CC = gcc
CFLAGS = -std=gnu17 -march=native -mtune=native -flto -O2 -Wall -Wextra
LDLIBS = -lm -luuid -lpthread -lvulkan -lglfw

# Shader Compiler Options
SLC = glslc
SLFLAGS = -O

# Targets
SOURCES := $(wildcard *.c)
VSHADERS := $(wildcard shaders/*.vert)
FSHADERS := $(wildcard shaders/*.frag)
OBJECTS := $(SOURCES:%.c=%.o)
MODULES := $(VSHADERS:%.vert=%.spv) $(FSHADERS:%.frag=%.spv)
EXECUTABLE := zeroClient

# Meta Rules
.PHONY: all clean
all: $(EXECUTABLE) $(MODULES) $(VSHADERS) $(FSHADERS)
clean:
	-rm $(OBJECTS) $(EXECUTABLE)

# Main Program
$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(LDLIBS) $(CFLAGS)
zeroClient.o: zeroClient.c platformBase.h networkSystem.h renderSystem.h contentManager.h gameLogic.h
	$(CC) -c $< $(CFLAGS)

# Game Logic
gameLogic.o: gameLogic.c gameLogic.h
	$(CC) -c $< $(CFLAGS)

# Windowing and Controls
platformBase.o: platformBase.c platformBase.h
	$(CC) -c $< $(CFLAGS)

# Network System
networkSystem.o: networkSystem.c networkSystem.h
	$(CC) -c $< $(CFLAGS)

# Content Manager
contentManager.o: contentManager.c contentManager.h
	$(CC) -c $< $(CFLAGS)

# Rendering System
renderSystem.o: renderSystem.c renderSystem.h renderSystemCore.h renderSystemDevice.h renderSystemMemory.h renderSystemSwapchain.h
	$(CC) -c $< $(CFLAGS)
renderSystemCore.o: renderSystemCore.c renderSystemCore.h platformBase.h
	$(CC) -c $< $(CFLAGS)
renderSystemDevice.o: renderSystemDevice.c renderSystemDevice.h
	$(CC) -c $< $(CFLAGS)
renderSystemMemory.o: renderSystemMemory.c renderSystemMemory.h
	$(CC) -c $< $(CFLAGS)
renderSystemSwapchain.o: renderSystemSwapchain.c renderSystemSwapchain.h
	$(CC) -c $< $(CFLAGS)

# Shaders
shaders/vertex.spv: shaders/vertex.vert
	$(SLC) $< -o $@ $(SLFLAGS)
shaders/fragment.spv: shaders/fragment.frag
	$(SLC) $< -o $@ $(SLFLAGS)
