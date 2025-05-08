#include "network.h"

#include "logger.h"

int32_t server;

void initNetwork() {
    server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(5316),
        .sin_addr = {
            .s_addr = inet_addr("127.0.0.1")
        },
        .sin_zero = {}
    };

    socklen_t addrlen = sizeof(addr);

    int32_t retval = connect(server, (struct sockaddr *) &addr, addrlen);
    assert(retval == 0);

    debug("Connected to the server:");
    debug("\tIP:   %s",  inet_ntoa(addr.sin_addr));
    debug("\tPort: %hu", ntohs(addr.sin_port));
}

void loopNetwork() {
    //while(1);
}

void quitNetwork() {
    close(server);
    debug("Discconected from server");
}
