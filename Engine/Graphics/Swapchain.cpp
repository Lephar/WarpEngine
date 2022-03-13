module;

#include "Graphics.hpp"

module Engine:Graphics;

import :System;

namespace Engine::Graphics {
	extern vk::Device device;
	extern vk::SurfaceKHR surface;

	Swapchain::Swapchain(unsigned int imageCount, unsigned int activeImageCount, Memory& imageMemory) {
		unsigned int width = System::getWidth();
		unsigned int height = System::getHeight();

		depth = Image{ width, height, 1, vk::SampleCountFlagBits::e1, vk::Format::eD32SfloatS8Uint,
			vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth |
			vk::ImageAspectFlagBits::eStencil, imageMemory
		};
		
		color = Image{ width, height, 1, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb,
			vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, imageMemory
		};

		vk::SwapchainCreateInfoKHR swapchainInfo{
			.surface = surface,
			.minImageCount = imageCount,
			.imageFormat = vk::Format::eB8G8R8A8Srgb,
			.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
			.imageExtent = vk::Extent2D {
				.width = width,
				.height = height
			},
			.imageArrayLayers = 1,
			.imageUsage =
				vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = vk::PresentModeKHR::eMailbox,
			.clipped = true,
			.oldSwapchain = nullptr
		};

		swapchain = device.createSwapchainKHR(swapchainInfo);
		std::vector<vk::Image> imageHandles = device.getSwapchainImagesKHR(swapchain);

		for (vk::Image& imageHandle : imageHandles)
			images.emplace_back(imageHandle, vk::Format::eB8G8R8A8Srgb, vk::ImageAspectFlagBits::eColor, imageMemory);
	}

	void Swapchain::destroy() {
		color.destroy();
		depth.destroy();

		for (Image& image : images)
			device.destroyImageView(image.getView());

		device.destroySwapchainKHR(swapchain);
	}
}