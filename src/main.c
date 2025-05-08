#include "main.h"

int main(int argc, char *argv[]) {
    initNetwork();
    initEngine(argc, argv);

    loopNetwork();
    loopEngine();

    quitNetwork();
    quitEngine();

    return EXIT_SUCCESS;
}
