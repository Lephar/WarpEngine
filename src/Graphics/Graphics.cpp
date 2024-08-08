#include "Graphics/Graphics.hpp"
#include "Graphics/Instance.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    vk::raii::Context context;
    std::vector<Instance *> instances;

    Instance *createInstance(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions) {
        auto instance = new Instance(context, title, layers, extensions);
        instances.push_back(instance);
        return instance;
    }

    Instance *getInstance(size_t index) {
        return instances.at(index);
    }

    Instance *getDefaultInstance() {
        return instances.front();
    }

    void destroy() {
        for(auto instance : instances) {
            delete instance;
        }
    }
}
