module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;

	Queue::Queue(unsigned int queueFamilyIndex) {
		queue = device.getQueue(queueFamilyIndex, 0);

		vk::CommandPoolCreateInfo commandPoolInfo{
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = queueFamilyIndex
		};

		commandPool = device.createCommandPool(commandPoolInfo);
	}

	void Queue::allocateCommandBuffers(unsigned int count) {
		vk::CommandBufferAllocateInfo commandBufferInfo{
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = count
		};

		commandBuffers = device.allocateCommandBuffers(commandBufferInfo);
	}

	vk::CommandBuffer& Queue::getCommandBuffer(unsigned int index) {
		return commandBuffers.at(index);
	}

	void Queue::waitIdle() {
		queue.waitIdle();
	}

	void Queue::freeCommandBuffers() {
		device.freeCommandBuffers(commandPool, commandBuffers);
	}

	void Queue::destroy() {
		device.destroyCommandPool(commandPool);
	}
}