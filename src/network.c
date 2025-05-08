#include "network.h"

#include "logger.h"

void initNetwork() {
    int32_t server = socket(AF_INET, SOCK_STREAM, 0);

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

    char *ip = inet_ntoa(addr.sin_addr);
    uint16_t port = ntohs(addr.sin_port);

    debug("IP:   %s", ip);
    debug("Port: %d", port);

    while(1);
}

void loopNetwork() {

}

void quitNetwork() {

}
