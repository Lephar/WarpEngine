#include "game.hpp"

namespace zero::game {
    const char *uuidFile = "data";

    Game::Game() : uuid() {
        if(std::filesystem::exists(uuidFile)) {
            std::fstream file{uuidFile, std::ios_base::in | std::ios_base::binary};
            file.read(reinterpret_cast<char *>(&uuid), sizeof(uuid_t));
        } else {
            uuid_generate(uuid);

            std::fstream file{uuidFile, std::ios_base::out | std::ios_base::binary};
            file.write(reinterpret_cast<char *>(&uuid), sizeof(uuid_t));
        }
    }
}