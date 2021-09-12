#ifndef ZERO_CLIENT_NETWORK_SERVER_HPP
#define ZERO_CLIENT_NETWORK_SERVER_HPP

#include "base.hpp"

namespace zero::network {
    class Server {
    private:
        int32_t server;
        sockaddr_in address;

        bool active;
        pthread_rwlock_t lock; // TODO: Use atomics

        pthread_t receiver;
        pthread_t sender;
    public:
        Server(const char *ip, uint32_t port);

        bool isActive();

        void setActive(bool value);

        void receiveLoop();

        void sendLoop();

        void connect();

        void disconnect();
    };
}

#endif //ZERO_CLIENT_NETWORK_SERVER_HPP
