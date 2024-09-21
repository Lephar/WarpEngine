#pragma once

#include <vector>

namespace Graphics {
    class Instance;

    Instance *createInstance(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions);
    Instance *getInstance(size_t index);
    Instance *getDefaultInstance();
    void destroy();
}
