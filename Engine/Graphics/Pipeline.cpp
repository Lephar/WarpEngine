#include "Graphics.hpp"

namespace Engine::Graphics {
	extern PhysicalDevice physicalDevice;
	extern vk::Device device;
	extern Swapchain swapchain;
	
	shaderc::Compiler shaderCompiler;
	shaderc::CompileOptions shaderOptions;

	Pipeline pipeline;

	void initializePipelineDetails() {
		pipeline.mipLevels = 8;
		pipeline.maxAnisotropy = physicalDevice.properties.limits.maxSamplerAnisotropy;

		//shaderOptions.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);
	}

	vk::ShaderModule loadShader(const char* name) {
		auto path = std::filesystem::current_path() / "Shaders" / name;
		auto size = std::filesystem::file_size(path);

		shaderc_shader_kind kind = shaderc_shader_kind::shaderc_glsl_compute_shader;

		if (!path.extension().compare(".vert"))
			kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
		else if (!path.extension().compare(".frag"))
			kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;

		std::ifstream file(path);
		std::vector<char> code(size);
		file.read(code.data(), size);

		auto shaderModule = shaderCompiler.CompileGlslToSpv(code.data(), code.size(), kind, name, "main", shaderOptions);

#ifndef NDEBUG
		if (shaderModule.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success) {
			if (!shaderModule.GetErrorMessage().empty())
				std::cout << "SHADERC Error: " << shaderModule.GetErrorMessage() << std::endl;
			return nullptr;
		}
#endif

		std::vector<unsigned> data(shaderModule.cbegin(), shaderModule.cend());

		vk::ShaderModuleCreateInfo shaderInfo{
			.codeSize = static_cast<unsigned>(data.size() * sizeof(unsigned)),
			.pCode = data.data()
		};

		return device.createShaderModule(shaderInfo);
	}

	void createLayouts() {
		std::array<vk::DescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings{
			vk::DescriptorSetLayoutBinding{
				.binding = 0,
				.descriptorType = vk::DescriptorType::eUniformBufferDynamic,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eVertex,
				.pImmutableSamplers = nullptr
			},
			vk::DescriptorSetLayoutBinding{
				.binding = 1,
				.descriptorType = vk::DescriptorType::eCombinedImageSampler,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eFragment,
				.pImmutableSamplers = nullptr
			}
		};

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
			.bindingCount = static_cast<unsigned>(descriptorSetLayoutBindings.size()),
			.pBindings = descriptorSetLayoutBindings.data()
		};

		pipeline.descriptorSetLayout = device.createDescriptorSetLayout(descriptorSetLayoutInfo);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
			.setLayoutCount = 1,
			.pSetLayouts = &pipeline.descriptorSetLayout,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		pipeline.pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
	}

	void createSampler() {
		vk::SamplerCreateInfo samplerInfo{
			.magFilter = vk::Filter::eLinear,
			.minFilter = vk::Filter::eLinear,
			.mipmapMode = vk::SamplerMipmapMode::eLinear,
			.addressModeU = vk::SamplerAddressMode::eRepeat,
			.addressModeV = vk::SamplerAddressMode::eRepeat,
			.addressModeW = vk::SamplerAddressMode::eRepeat,
			.mipLodBias = 0.0f,
			.anisotropyEnable = false,
			.maxAnisotropy = pipeline.maxAnisotropy,
			.compareEnable = false,
			.compareOp = vk::CompareOp::eAlways,
			.minLod = 0.0f,
			.maxLod = 1.0f,
			.borderColor = vk::BorderColor::eIntOpaqueBlack,
			.unnormalizedCoordinates = false,
		};

		pipeline.sampler = device.createSampler(samplerInfo);
	}

	void createPipelineObject() {
		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{ 
			vk::PipelineShaderStageCreateInfo{
				.stage = vk::ShaderStageFlagBits::eVertex,
				.module = pipeline.vertexShader,
				.pName = "main",
				.pSpecializationInfo = nullptr
			},
			vk::PipelineShaderStageCreateInfo{
				.stage = vk::ShaderStageFlagBits::eFragment,
				.module = pipeline.fragmentShader,
				.pName = "main",
				.pSpecializationInfo = nullptr
			},
		};

		vk::VertexInputBindingDescription bindingDescription{
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex
		};
		
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{
			vk::VertexInputAttributeDescription{
				.location = 0,
				.binding = 0,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(Vertex, position)
			},
			vk::VertexInputAttributeDescription{
				.location = 1,
				.binding = 0,
				.format = vk::Format::eR32G32Sfloat,
				.offset = offsetof(Vertex, texture)
			}
		};

		vk::PipelineVertexInputStateCreateInfo inputStateInfo{
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDescription,
			.vertexAttributeDescriptionCount = static_cast<unsigned>(attributeDescriptions.size()),
			.pVertexAttributeDescriptions = attributeDescriptions.data()
		};
		
		vk::PipelineInputAssemblyStateCreateInfo assemblyInfo{
			.topology = vk::PrimitiveTopology::eTriangleList,
			.primitiveRestartEnable = false
		};
		
		vk::PipelineRasterizationStateCreateInfo rasterizerInfo{
			.depthClampEnable = false,
			.rasterizerDiscardEnable = false,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = vk::CullModeFlagBits::eBack,
			.frontFace = vk::FrontFace::eClockwise,
			.depthBiasEnable = false,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f
		};
		
		vk::PipelineMultisampleStateCreateInfo multisamplingInfo{
			.rasterizationSamples = swapchain.sampleCount,
			.sampleShadingEnable = false,
			.minSampleShading = 1.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = false,
			.alphaToOneEnable = false 
		};

		vk::StencilOpState stencilOpState{
			.failOp = vk::StencilOp::eKeep,
			.passOp = vk::StencilOp::eKeep,
			.depthFailOp = vk::StencilOp::eKeep,
			.compareOp = vk::CompareOp::eEqual,
			.compareMask = 0xFF,
			.writeMask = 0xFF
		};

		vk::PipelineDepthStencilStateCreateInfo depthStencil{
			.depthTestEnable = true,
			.depthWriteEnable = true,
			.depthCompareOp = vk::CompareOp::eLessOrEqual,
			.depthBoundsTestEnable = false,
			.stencilTestEnable = true,
			.front = stencilOpState,
			.back = vk::StencilOpState{},
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f
		};
		
		vk::PipelineColorBlendAttachmentState blendAttachment{
			.blendEnable = true,
			.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
			.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
			.colorBlendOp = vk::BlendOp::eAdd,
			.srcAlphaBlendFactor = vk::BlendFactor::eOne,
			.dstAlphaBlendFactor = vk::BlendFactor::eZero,
			.alphaBlendOp = vk::BlendOp::eAdd,
			.colorWriteMask =
				vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA,
		};

		vk::PipelineColorBlendStateCreateInfo blendInfo{
			.logicOpEnable = VK_FALSE,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &blendAttachment,
			.blendConstants = std::array<float, 4>{
				0.0f,
				0.0f,
				0.0f,
				0.0f
			}
		};
		
		vk::Viewport viewport{
			.x = 0.0f,
			.y = static_cast<float>(swapchain.extent.height),
			.width = static_cast<float>(swapchain.extent.width),
			.height = -static_cast<float>(swapchain.extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		
		vk::Rect2D scissor{
			.offset = vk::Offset2D{
				.x = 0,
				.y = 0
			},
			.extent = swapchain.extent
		};
		
		vk::PipelineViewportStateCreateInfo viewportInfo{
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor
		};

		vk::DynamicState dynamicViewport = vk::DynamicState::eViewport;

		vk::PipelineDynamicStateCreateInfo dynamicStateInfo{
			.dynamicStateCount = 1,
			.pDynamicStates = &dynamicViewport
		};

		vk::PipelineRenderingCreateInfo renderingInfo{
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &swapchain.colorFormat,
			.depthAttachmentFormat = swapchain.depthStencilFormat,
			.stencilAttachmentFormat = swapchain.depthStencilFormat
		};

		vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
			.pNext = &renderingInfo,
			.stageCount = static_cast<unsigned>(shaderStages.size()),
			.pStages = shaderStages.data(),
			.pVertexInputState = &inputStateInfo,
			.pInputAssemblyState = &assemblyInfo,
			.pViewportState = &viewportInfo,
			.pRasterizationState = &rasterizerInfo,
			.pMultisampleState = &multisamplingInfo,
			.pDepthStencilState = &depthStencil,
			.pColorBlendState = &blendInfo,
			.pDynamicState = &dynamicStateInfo,
			.layout = pipeline.pipelineLayout,
			.renderPass = nullptr,
			.subpass = 0,
			.basePipelineHandle = nullptr,
			.basePipelineIndex = 0
		};

		pipeline.pipeline = device.createGraphicsPipeline(nullptr, graphicsPipelineInfo).value;
	}

	void createPipeline() {
		initializePipelineDetails();

		pipeline.vertexShader = loadShader("vertex.vert");
		pipeline.fragmentShader = loadShader("fragment.frag");

		createLayouts();
		createSampler();
		createPipelineObject();
	}

	void destroyPipeline() {
		device.destroyPipeline(pipeline.pipeline);

		device.destroySampler(pipeline.sampler);

		device.destroyPipelineLayout(pipeline.pipelineLayout);
		device.destroyDescriptorSetLayout(pipeline.descriptorSetLayout);

		device.destroyShaderModule(pipeline.fragmentShader);
		device.destroyShaderModule(pipeline.vertexShader);
	}
}