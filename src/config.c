#include "config.h"

#include "window.h"

#include "file.h"
#include "logger.h"

uint32_t threadCount = 0;

void configure(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    threadCount = get_nprocs();
    assert(threadCount);
    debug("Threads: %u", threadCount);

    char *separator = strrchr(argv[0], '/');

    char *result = nullptr;
    int32_t length = 0;

    if(separator == nullptr) {
        result = getcwd(rootPath, PATH_MAX);
        assert(result != nullptr);

        length = snprintf(executableName, PATH_MAX, "%s", argv[0]);
        assert(length < PATH_MAX);
    } else {
        *separator++ = '\0'; // Let's play a game

        length = snprintf(rootPath, PATH_MAX, "%s", argv[0]);
        assert(length < PATH_MAX);

        length = snprintf(executableName, PATH_MAX, "%s", separator);
        assert(length < PATH_MAX);
    }

    debug("Path:    %s", rootPath);
    debug("Name:    %s", executableName);

    char config[PATH_MAX];

    if(argc < 2) {
        makeFullPath(nullptr, "config.txt", config);
    } else {
        makeFullPath(nullptr, argv[1],      config);
    }

    debug("Config:  %s", config);

    FILE *file = fopen(config, "r");
    assert(file != nullptr);

    char discard[PATH_MAX];
    fscanf(file, "%s", discard);
    assert(strncasecmp(discard, "Window:", PATH_MAX) == 0);

    fscanf(file, "%u%u", &extent.width, &extent.height);
    debug("Width:   %u", extent.width );
    debug("Height:  %u", extent.height);

    fclose(file);
}
