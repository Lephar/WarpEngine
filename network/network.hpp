#ifndef ZERO_CLIENT_NETWORK_HPP
#define ZERO_CLIENT_NETWORK_HPP

#include "base.hpp"
#include "server.hpp"

namespace zero::network {
    class Network {
    private:
        std::unordered_map<const char *, Server> servers;
    public:
        Server &addServer(const char *name, const char *ip, uint32_t port);
        ~Network();
    };
}

#endif //ZERO_CLIENT_NETWORK_HPP
