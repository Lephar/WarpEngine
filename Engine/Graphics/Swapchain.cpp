#include "Graphics.hpp"

namespace Engine::Graphics {
	extern vk::SurfaceKHR surface;
	extern PhysicalDevice physicalDevice;
	extern vk::Device device;
	extern Memory deviceMemory;

	Swapchain swapchain;

	// TODO: Better swapchain details initialization
	void initializeSwapchainDetails() {
		swapchain.imageCount = 3;
		swapchain.framebufferCount = 2;

		swapchain.depthStencilFormat = vk::Format::eD32SfloatS8Uint;
		swapchain.colorFormat = vk::Format::eA2B10G10R10UnormPack32;
		swapchain.presentMode = vk::PresentModeKHR::eMailbox;
		swapchain.sampleCount = vk::SampleCountFlagBits::e8;

		swapchain.surfaceFormat = vk::SurfaceFormatKHR{
			.format = vk::Format::eA2B10G10R10UnormPack32,
			.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear
		};
	}

	void createFramebuffers() {
		for (unsigned framebufferIndex = 0; framebufferIndex < swapchain.imageCount; framebufferIndex++) {
			Framebuffer framebuffer;

			framebuffer.depthStencil = createImage(deviceMemory, swapchain.extent.width, swapchain.extent.height, 1, swapchain.sampleCount, swapchain.depthStencilFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
			framebuffer.color = createImage(deviceMemory, swapchain.extent.width, swapchain.extent.height, 1, swapchain.sampleCount, swapchain.colorFormat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment, vk::ImageAspectFlagBits::eColor);
			framebuffer.resolve = createImage(deviceMemory, swapchain.extent.width, swapchain.extent.height, 1, swapchain.sampleCount, swapchain.colorFormat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc, vk::ImageAspectFlagBits::eColor);
	
			swapchain.framebuffers.push_back(framebuffer);
		}
	}

	void createSwapchain(unsigned width, unsigned height) {
		swapchain.extent = vk::Extent2D{
			.width = width,
			.height = height
		};
		
		initializeSwapchainDetails();

		vk::SwapchainCreateInfoKHR swapchainInfo{
			.surface = surface,
			.minImageCount = swapchain.imageCount,
			.imageFormat = swapchain.surfaceFormat.format,
			.imageColorSpace = swapchain.surfaceFormat.colorSpace,
			.imageExtent = swapchain.extent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = physicalDevice.surfaceCapabilities.currentTransform,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = swapchain.presentMode,
			.clipped = true,
			.oldSwapchain = nullptr
		};

		swapchain.swapchain = device.createSwapchainKHR(swapchainInfo);

		createFramebuffers();
	}

	void recreateSwapchain(unsigned width, unsigned height) {
		destroySwapchain();
		createSwapchain(width, height);
	}

	void destroySwapchain() {
		for (auto& framebuffer : swapchain.framebuffers) {
			destroyImage(framebuffer.resolve);
			destroyImage(framebuffer.color);
			destroyImage(framebuffer.depthStencil);
		}

		device.destroySwapchainKHR(swapchain.swapchain);
	}
}
