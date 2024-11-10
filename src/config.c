#include "config.h"

#include "helper.h"
#include "queue.h"
#include "shader.h"
#include "manager.h"

char rootPath[PATH_MAX];
char executableName[PATH_MAX];

extern VkExtent2D extent;

extern Queue graphicsQueue;
extern Queue  computeQueue;
extern Queue transferQueue;

Queue *queueReferences[] = {
    &graphicsQueue,
    & computeQueue,
    &transferQueue
};

uint32_t queueCount;

extern Shader vertex;
extern Shader fragment;

Shader *shaderReferences[] = {
    &vertex,
    &fragment
};

uint32_t shaderCount;

void configure(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    char *separator = strrchr(argv[0], '/');

    if(separator == NULL) {
        assert(getcwd(rootPath, PATH_MAX) != NULL);
        assert(snprintf(executableName, PATH_MAX, "%s", argv[0]) < PATH_MAX);
    } else {
        *separator++ = '\0'; // Let's play a game
        assert(snprintf(rootPath, PATH_MAX, "%s", argv[0]) < PATH_MAX);
        assert(snprintf(executableName, PATH_MAX, "%s", separator) < PATH_MAX);
    }

    debug("Path:   %s", rootPath);
    debug("Name:   %s", executableName);

    char config[PATH_MAX];

    if(argc < 2) {
        assert(snprintf(config, PATH_MAX, "%s/config.txt", rootPath) < PATH_MAX);
    } else {
        assert(snprintf(config, PATH_MAX, "%s/%s", rootPath, argv[1]) < PATH_MAX);
    }

    debug("Config: %s", config);

    FILE *file = fopen(config, "r");
    assert(file != NULL);

    char discard[PATH_MAX];
    fscanf(file, "%s", discard);
    assert(strncmp(discard, "Window:", PATH_MAX) == 0 || strncmp(discard, "Fullscreen:", PATH_MAX) == 0);

    fscanf(file, "%d%d", &extent.width, &extent.height);
    debug("Width:  %d", extent.width );
    debug("Height: %d", extent.height);

    // Discard rest of the file for now
    fclose(file);
}
