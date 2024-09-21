#include "System/System.hpp"
#include "System/Window.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"

int main(int argc, char* args[])
{
    static_cast<void>(argc);
    static_cast<void>(args);

    auto window = System::initialize("Zero", 800, 600);
    auto instance = Graphics::createInstance("Zero", std::vector<const char *>{}, window->getExtensions());
    auto device = instance->getDefaultDevice();

    auto surface = window->createSurface(**instance->getInstanceHandle());
    device->registerSurface(vk::raii::SurfaceKHR{*instance->getInstanceHandle(), surface}, window->getExtent());

    Graphics::destroy();
    System::quit();

    return EXIT_SUCCESS;
}
