#include "server.hpp"

namespace zero::network {
    void optimizeSocket(int32_t socket) {
        int32_t nodelayValue = 1;
        uint8_t tosValue = IPTOS_LOWDELAY;

        setsockopt(socket, IPPROTO_IP, IP_TOS, &tosValue, sizeof(uint8_t));
        setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &nodelayValue, sizeof(int32_t));
    }

    Server::Server(const char *ip, uint32_t port) : address(), lock(), receiver(), sender() {
        active = false;
        pthread_rwlock_init(&lock, nullptr);

        server = socket(AF_INET, SOCK_STREAM, 0);
        optimizeSocket(server);

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, ip, &address.sin_addr);
    }

    bool Server::isActive() {
        pthread_rwlock_rdlock(&lock);
        bool value = active;
        pthread_rwlock_unlock(&lock);

        return value;
    }

    void Server::setActive(bool value) {
        pthread_rwlock_wrlock(&lock);
        active = value;
        pthread_rwlock_unlock(&lock);
    }

    void *startReceiver(void *context) {
        auto server = reinterpret_cast<Server *>(context);

        while (server->isActive()) {
            // Process updates here
        }

        return nullptr;
    }

    void *startSender(void *context) {
        auto server = reinterpret_cast<Server *>(context);

        while (server->isActive()) {
            // Send updates here
        }

        return nullptr;
    }

    void Server::connect() {
        static_cast<void>(::connect(server, reinterpret_cast<sockaddr *>(&address), sizeof(sockaddr_in)));

        setActive(true);

        pthread_create(&receiver, nullptr, startReceiver, this);
        pthread_create(&sender, nullptr, startSender, this);
    }

    void Server::disconnect() {
        setActive(false);

        pthread_join(receiver, nullptr);
        pthread_join(sender, nullptr);

        close(server);
    }
}