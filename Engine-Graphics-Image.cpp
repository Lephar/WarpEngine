module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;

	Image::Image(unsigned int imageWidth, unsigned int imageHeight, unsigned int mipLevels,
		vk::SampleCountFlagBits sampleCount, vk::Format imageFormat, vk::ImageUsageFlags usageFlags,
		Memory& imageMemory) : width(imageWidth), height(imageHeight), levels(mipLevels),
		memory(imageMemory.getMemoryHandle()) {
		vk::ImageCreateInfo imageInfo{
			.imageType = vk::ImageType::e2D,
			.format = imageFormat,
			.extent = vk::Extent3D {
				.width = imageWidth,
				.height = imageHeight,
				.depth = 1
			},
			.mipLevels = mipLevels,
			.arrayLayers = 1,
			.samples = sampleCount,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = usageFlags,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined
		};

		image = device.createImage(imageInfo);
		imageMemory.bindImage(image);
	}
}
