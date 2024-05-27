#pragma once

#include <vector>

namespace Graphics {
    class Device;

    Device *initialize(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions);
    Device *getDevice(size_t index);
    Device *getDefaultDevice(void);
    void destroy(void);
}
