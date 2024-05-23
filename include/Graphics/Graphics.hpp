#pragma once

#include <vector>

namespace Graphics {
    class Device;

    void initialize(const char *title, std::vector<const char *> extensions);
    Device *getDevice(size_t index = 0);
    void destroy(void);
}
