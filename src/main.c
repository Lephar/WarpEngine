#include "main.h"

#include "config.h"

#include "graphics.h"

int main(int argc, char *argv[]) {
    configure(argc, argv);

    initEngine();
    loopEngine();
    quitEngine();

    return EXIT_SUCCESS;
}
