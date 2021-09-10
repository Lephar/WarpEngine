#include "network.hpp"

namespace zero::network {
    Server &Network::addServer(const char *name, const char *ip, uint32_t port) {
        return servers.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(ip, port)).first->second;
    }

    Network::~Network() {
        for(auto& server : servers)
            server.second.disconnect();
    }
}
