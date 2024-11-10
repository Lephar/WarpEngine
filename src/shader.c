#include "shader.h"

#include "helper.h"

extern VkDevice device;

Shader vertex;
Shader fragment;

VkDescriptorPool descriptorPool;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorSet descriptorSet;

void createModule(Shader *shader, const char *name, shaderc_shader_kind kind) {
    shader->name = name;
    shader->kind = kind;

    debug("Shader: %s", shader->name);
    debug("\tKind: %d", shader->kind);

    const char *extension = "glsl";

    if(shader->kind == shaderc_compute_shader) {
        extension = "comp";
    } else if(shader->kind == shaderc_vertex_shader) {
        extension = "vert";
    } else if(shader->kind == shaderc_fragment_shader) {
        extension = "frag";
    } //TODO: Add other shader types

    sprintf(shader->file, "shaders/%s.%s.spv", shader->name, extension);
    debug("\tPath: %s", shader->file);

    readFile(shader->file, 1, &shader->size, (char**)&shader->intermediate);
    debug("\tSize: %ld", shader->size);

    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shader->size,
        .pCode = shader->intermediate
    };

    vkCreateShaderModule(device, &shaderInfo, NULL, &shader->module);
    debug("Successfully created");
}

void createModules() {
    //createModule(&vertex,   "vertex"  , shaderc_vertex_shader  );
    //createModule(&fragment, "fragment", shaderc_fragment_shader);

    createModule(&vertex,   "vertex_fixed"  , shaderc_vertex_shader  );
    createModule(&fragment, "fragment_fixed", shaderc_fragment_shader);
}

void createDescriptors() {
    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool);
    debug("Descriptor pool created");

    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = &layoutBinding
    };

    vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout);
    debug("Descriptor set layout created");

    VkDescriptorSetAllocateInfo setInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    vkAllocateDescriptorSets(device, &setInfo, &descriptorSet);
    debug("Descriptor set allocated");
}

void destroyModule(Shader *shader) {
    vkDestroyShaderModule(device, shader->module, NULL);

    free(shader->code);
    shader->code = NULL;

    debug("Shader module %s destroyed", shader->name);
    shader->name = NULL;
}

void destroyModules() {
    destroyModule(&fragment);
    destroyModule(&vertex);
}

void destroyDescriptors() {
    vkDestroyDescriptorPool(device, descriptorPool, NULL);
    debug("Descriptor pool destroyed");
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    debug("Descriptor set layout destroyed");
}
