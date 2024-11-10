#include "shader.h"

#include "helper.h"

shaderc_compiler_t shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

extern VkDevice device;
Shader vertex;
Shader fragment;

Shader *shaderReferences[] = {
    &vertex,
    &fragment
};

VkDescriptorPool descriptorPool;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorSet descriptorSet;

void loadShader(Shader *shader, const char *name, shaderc_shader_kind kind, VkBool32 intermediate) {
    debug("Shader: %s", name);
    debug("\tKind:          %d", kind);
    debug("\tIntermediate:  %d", intermediate);

    const char *extension = ".glsl";

    if(kind == shaderc_compute_shader) {
        extension = ".comp";
    } else if(kind == shaderc_vertex_shader) {
        extension = ".vert";
    } else if(kind == shaderc_fragment_shader) {
        extension = ".frag";
    } //TODO: Add other shader types

    char shaderFile[PATH_MAX];
    sprintf(shaderFile, "shaders/%s%s%s", name, extension, intermediate ? ".spv" : "");
    debug("\tPath:          %s", shaderFile);

    if(intermediate) {
        readFile(shaderFile, 1, &shader->size, &shader->data);
        debug("\tSize:          %ld", shader->size);
    } else {
        size_t shaderCodeSize;
        char *shaderCode;

        readFile(shaderFile, 0, &shaderCodeSize, &shaderCode);
        debug("\tCode size:     %ld", shaderCodeSize);

        shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode, shaderCodeSize - 1, kind, shaderFile, "main", shaderCompileOptions);

        if(shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
            debug("%s", shaderc_result_get_error_message(result));
            shaderc_result_release(result);
            assert(0);
        }

        shader->size = shaderc_result_get_length(result);
        shader->data = malloc(shader->size);

        memcpy(shader->data, shaderc_result_get_bytes(result), shader->size);
        debug("\tCompiled size: %ld", shader->size);

        shaderc_result_release(result);
        free(shaderCode);
    }
}

void loadShaders() {
    shaderCompiler = shaderc_compiler_initialize();
    // TODO: Add actual compiler options depending on compilation profile
    shaderCompileOptions = shaderc_compile_options_initialize();
    debug("Shader compiler and shader compile options set");

    loadShader(&vertex  , "vertex_fixed"  , shaderc_vertex_shader  , VK_TRUE);
    loadShader(&fragment, "fragment_fixed", shaderc_fragment_shader, VK_TRUE);
}

void createModule(Shader *shader) {
    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shader->size,
        .pCode = (uint32_t *)shader->data
    };

    vkCreateShaderModule(device, &shaderInfo, NULL, &shader->module);
}

void createModules() {
    createModule(&vertex  );
    createModule(&fragment);
    debug("Shader modules created");
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

    free(shader->data);
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    destroyModule(&fragment);
    destroyModule(&vertex  );
}

void destroyDescriptors() {
    vkDestroyDescriptorPool(device, descriptorPool, NULL);
    debug("Descriptor pool destroyed");

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    debug("Descriptor set layout destroyed");
}
