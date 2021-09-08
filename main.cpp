#include "system/system.hpp"
#include "graphics/graphics.hpp"

int main() {
    zero::system::System system;
    auto connection = system.getConnection();
    auto window = system.createWindow("Zero", 800, 600);

    zero::graphics::Graphics graphics;

    system.mainLoop();

    return 0;
}
