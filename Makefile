CC = gcc
LDLIBS = -lm -luuid -lpthread -lvulkan -lglfw
CFLAGS = -std=gnu17 -march=native -mtune=native -Wall -Wextra
DBGFLAGS = -g -Og
RELFLAGS = -flto -O2 -DNDEBUG

SLC = glslc
SLFLAGS = -O

MKDIR = mkdir
MKFLAGS = -p

SOURCEDIR = source
SHADERDIR = shaders
OBJECTDIR = build
MODULEDIR = modules

SOURCES := $(wildcard $(SOURCEDIR)/*.c)
COMPSHADERS := $(wildcard $(SHADERDIR)/*.comp)
VERTSHADERS := $(wildcard $(SHADERDIR)/*.vert)
FRAGSHADERS := $(wildcard $(SHADERDIR)/*.frag)

OUTDIRS := $(OBJECTDIR) $(MODULEDIR)
OBJECTS := $(SOURCES:$(SOURCEDIR)/%.c=$(OBJECTDIR)/%.o)
HEADERDEPS := $(OBJECTS:%.o=%.d)
COMPMODULES := $(COMPSHADERS:$(SHADERDIR)/%.comp=$(MODULEDIR)/%.comp.spv)
VERTMODULES := $(VERTSHADERS:$(SHADERDIR)/%.vert=$(MODULEDIR)/%.vert.spv)
FRAGMODULES := $(FRAGSHADERS:$(SHADERDIR)/%.frag=$(MODULEDIR)/%.frag.spv)
EXECUTABLE := zeroClient

.PHONY: all debug release clean

all: $(EXECUTABLE) $(COMPMODULES) $(VERTMODULES) $(FRAGMODULES)

debug: $(OUTDIRS)
	@$(MAKE) CFLAGS="$(CFLAGS) $(DBGFLAGS)" --no-print-directory

release: $(OUTDIRS)
	@$(MAKE) CFLAGS="$(CFLAGS) $(RELFLAGS)" --no-print-directory

$(OUTDIRS):
	@$(MKDIR) $(MKFLAGS) $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(LDLIBS) $(CFLAGS)

$(OBJECTS): $(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $< -o $@ -MMD -MF $(OBJECTDIR)/$*.d $(CFLAGS)

$(COMPMODULES): $(MODULEDIR)/%.comp.spv: $(SHADERDIR)/%.comp
	$(SLC) $< -o $@ $(SLFLAGS)

$(VERTMODULES): $(MODULEDIR)/%.vert.spv: $(SHADERDIR)/%.vert
	$(SLC) $< -o $@ $(SLFLAGS)

$(FRAGMODULES): $(MODULEDIR)/%.frag.spv: $(SHADERDIR)/%.frag
	$(SLC) $< -o $@ $(SLFLAGS)

-include $(HEADERDEPS)

clean:
	-rm $(EXECUTABLE) $(OBJECTS) $(HEADERDEPS) $(COMPMODULES) $(VERTMODULES) $(FRAGMODULES)
