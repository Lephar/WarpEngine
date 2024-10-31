#include "config.h"

#include "helper.h"

char config[PATH_MAX];

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
        getcwd(path, PATH_MAX);
        strncpy(name, argv[0], PATH_MAX);
    } else {
        strncpy(path, argv[0], separator - argv[0]);
        strncpy(name, separator + 1, PATH_MAX);
    }

    path[PATH_MAX - 1] = '\0';
    name[PATH_MAX - 1] = '\0';

    debug("Path:   %s", path);
    debug("Name:   %s", name);

    if(argc >= 2) {
        strncpy(config, argv[1], PATH_MAX - 1);
    } else {
        strncpy(config, "config.txt", PATH_MAX - 1);
    }

    config[PATH_MAX - 1] = '\0';

    debug("Config: %s", config);
}