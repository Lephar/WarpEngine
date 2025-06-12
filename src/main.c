#include "main.h"

int main(int argc, char *argv[]) {
    configure(argc, argv);

    //initNetwork();
    initEngine();

    //loopNetwork();
    loopEngine();

    //quitNetwork();
    quitEngine();

    return EXIT_SUCCESS;
}
