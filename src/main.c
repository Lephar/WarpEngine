#include "main.h"

#include "graphics.h"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    initEngine();
    loopEngine();
    quitEngine();

    return EXIT_SUCCESS;
}
