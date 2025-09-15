#include "main.h"

#include "config.h"
#include "content.h"
#include "network.h"
#include "graphics.h"

int main(int argc, char *argv[]) {
    configure(argc, argv);

    //startContentModule();
    //startNetworkModule();
    startGraphicsModule();
    //initEngine();
    //loopEngine();

    //stopGraphicsModule();
    //stopNetworkModule();
    //stopContentModule();

    return EXIT_SUCCESS;
}
