#include "system/system.hpp"
#include "graphics/graphics.hpp"
#include "network/network.hpp"
#include "game/game.hpp"

int main() {
    zero::system::System system;
    auto connection = system.getConnection();
    auto window = system.createWindow("Zero", 800, 600);

    zero::graphics::Graphics graphics;

    zero::network::Network network;
    auto &server = network.addServer("Game", "127.0.0.1", 7076);

    zero::game::Game game;
    system.mainLoop();

    return 0;
}
