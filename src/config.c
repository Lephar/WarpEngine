#include "config.h"

#include "helper.h"
#include "file.h"
#include "queue.h"
#include "shader.h"
#include "content.h"

char rootPath[PATH_MAX];
char executableName[PATH_MAX];

extern VkExtent2D extent;

extern uint32_t modelCount;
extern Model *models;

void configure(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    char *separator = strrchr(argv[0], '/');

    char *result = NULL;
    int32_t length = 0;

    if(separator == NULL) {
        result = getcwd(rootPath, PATH_MAX);
        assert(result != NULL);

        length = snprintf(executableName, PATH_MAX, "%s", argv[0]);
        assert(length < PATH_MAX);
    } else {
        *separator++ = '\0'; // Let's play a game

        length = snprintf(rootPath, PATH_MAX, "%s", argv[0]);
        assert(length < PATH_MAX);

        length = snprintf(executableName, PATH_MAX, "%s", separator);
        assert(length < PATH_MAX);
    }

    debug("Path:   %s", rootPath);
    debug("Name:   %s", executableName);

    char config[PATH_MAX];

    if(argc < 2) {
        length = snprintf(config, PATH_MAX, "%s/config.txt", rootPath);
        assert(length < PATH_MAX);
    } else {
        length = snprintf(config, PATH_MAX, "%s/%s", rootPath, argv[1]);
        assert(length < PATH_MAX);
    }

    debug("Config: %s", config);

    FILE *file = fopen(config, "r");
    assert(file != NULL);

    char discard[PATH_MAX];
    fscanf(file, "%s", discard);
    assert(strncasecmp(discard, "Window:", PATH_MAX) == 0);

    fscanf(file, "%u%u", &extent.width, &extent.height);
    debug("Width:  %u", extent.width );
    debug("Height: %u", extent.height);

    fclose(file);
}
