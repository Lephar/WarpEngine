module;

#include "Engine-Graphics.hpp"

export module Engine:Graphics;

namespace Engine::Graphics {
	class Queue {
	private:
		vk::Queue queue;
		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;
	public:
		Queue() = default;
		Queue(unsigned int queueFamilyIndex);
		void allocateCommandBuffers(unsigned int count);
		vk::CommandBuffer& getCommandBuffer(unsigned int index = 0);
		void waitIdle();
		void freeCommandBuffers();
		void destroy();
	};

	class Image;
	class Buffer;

	class Memory {
	private:
		vk::DeviceSize size;
		vk::DeviceSize offset;
		vk::DeviceMemory memory;
		public:
		Memory() = default;
		Memory(unsigned int typeIndex, vk::DeviceSize allocationSize);
		vk::DeviceSize alignOffset(vk::MemoryRequirements requirements);
		vk::DeviceMemory& getMemoryHandle();
		void free();
	};

	class Buffer {
	private:
		vk::DeviceSize size;
		vk::DeviceSize offset;
		vk::DeviceMemory& memory;
		vk::Buffer buffer;
		vk::BufferView view;
		void createBuffer(vk::BufferUsageFlags usageFlags);
		void bindBufferMemory(Memory& bufferMemory);
	public:
		Buffer() = default;
		Buffer(vk::BufferUsageFlags usageFlags, vk::DeviceSize bufferSize, Memory& bufferMemory);
		void destroy();
	};

	class Image {
	private:
		vk::Image image;
		vk::ImageView view;
		void createImage(unsigned int imageWidth, unsigned int imageHeight, unsigned int mipLevels,
			vk::SampleCountFlagBits sampleCount, vk::Format imageFormat, vk::ImageUsageFlags usageFlags);
		void createImageView(unsigned int mipLevels, vk::Format imageFormat, vk::ImageAspectFlags aspectFlags);
		void bindImageMemory(Memory& imageMemory);
	private:
		Image() = default;
		Image(unsigned int imageWidth, unsigned int imageHeight, unsigned int mipLevels,
			vk::SampleCountFlagBits sampleCount, vk::Format imageFormat, vk::ImageUsageFlags usageFlags,
			vk::ImageAspectFlags aspectFlags, Memory& imageMemory);
		void destroy();
	};

	class Swapchain {
	private:
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> swapchainImages;
		std::vector<vk::ImageView> swapchainImageViews;
	public:
		Swapchain() = default;
		Swapchain(unsigned int imageCount, unsigned int activeImageCount);
		void destroy();
	};

	void initialize();
	void terminate();
}
