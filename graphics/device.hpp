#ifndef ZERO_CLIENT_GRAPHICS_DEVICE_HPP
#define ZERO_CLIENT_GRAPHICS_DEVICE_HPP

#include "header.hpp"
#include "instance.hpp"
#include "internals.hpp"
#include "details.hpp"

namespace zero::graphics {
	class Device {
	private:
		vk::Device device;

		vk::Queue transferQueue;
		vk::CommandPool transferCommandPool;
		vk::CommandBuffer transferCommandBuffer;

		std::vector<vk::Queue> graphicsQueues;
		std::vector<vk::CommandPool> graphicsCommandPools;

	public:
		Device(Instance &instance, Internals &internals, Details &details);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_DEVICE_HPP
