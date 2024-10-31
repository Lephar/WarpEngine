#include "config.h"

#include "helper.h"

extern char path[];
extern char name[];

extern VkExtent2D extent;

void configure(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    char *separator = strrchr(argv[0], '/');

    if(separator == NULL) {
        assert(getcwd(path, PATH_MAX) != NULL);
        assert(snprintf(name, PATH_MAX, "%s", argv[0]) < PATH_MAX);
    } else {
        *separator++ = '\0'; // Let's play a game
        assert(snprintf(path, PATH_MAX, "%s", argv[0]) < PATH_MAX);
        assert(snprintf(name, PATH_MAX, "%s", separator) < PATH_MAX);
    }

    debug("Path:   %s", path);
    debug("Name:   %s", name);

    char config[PATH_MAX];

    if(argc < 2) {
        assert(snprintf(config, PATH_MAX, "%s/config.txt", path) < PATH_MAX);
    } else {
        assert(snprintf(config, PATH_MAX, "%s/%s", path, argv[1]) < PATH_MAX);
    }

    debug("Config: %s", config);
}