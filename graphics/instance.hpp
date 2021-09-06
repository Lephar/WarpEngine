#ifndef ZERO_CLIENT_GRAPHICS_INSTANCE_HPP
#define ZERO_CLIENT_GRAPHICS_INSTANCE_HPP

#include "header.hpp"

namespace zero::graphics {
	class Instance {
	private:
		vk::Instance instance;
#ifndef NDEBUG
		vk::DispatchLoaderDynamic loader;
		vk::DebugUtilsMessengerEXT messenger;
#endif //NDEBUG
	public:
		Instance(const char* application, const char* engine, uint32_t version);
		vk::Instance &get(void);
		void destroy(void);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_INSTANCE_HPP
