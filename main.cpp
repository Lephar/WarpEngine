#include "system/system.hpp"
#include "graphics/graphics.hpp"
#include "network/network.hpp"
#include "game/game.hpp"

int main() {
    zero::system::System system;
    auto connection = system.getConnection();
    auto window = system.createWindow("Zero", 800, 600);
    auto properties = system.getWindowProperties(window);

    zero::graphics::Graphics graphics;
    auto &renderer = graphics.createRenderer(connection, window, properties);

    zero::network::Network network;
    auto &server = network.addServer("Game", "127.0.0.1", 7076);
    server.connect();

    zero::game::Game game;
    system.mainLoop();

    return 0;
}
