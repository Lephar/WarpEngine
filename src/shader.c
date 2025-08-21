#include "shader.h"

#include "device.h"
#include "descriptor.h"

#include "file.h"
#include "logger.h"

shaderc_compiler_t        shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

ShaderModule *vertexShaderModule;
ShaderModule *fragmentShaderModule;

ShaderCode *loadShaderCode(const char *subdirectory, const char *filename, shaderc_shader_kind stage) {
    ShaderCode *shaderCode = malloc(sizeof(ShaderCode));
    shaderCode->stage = stage;
    shaderCode->data  = NULL;
    shaderCode->size  = loadTextFile(subdirectory, filename, &shaderCode->data);

    debug("%s shader code with %lu bytes of size loaded successfully", shaderCode->stage == shaderc_vertex_shader ? "Vertex" : "Fragment", shaderCode->size);

    return shaderCode;
}

void freeShaderCode(ShaderCode *shaderCode) {
    shaderCode->stage = shaderc_glsl_infer_from_source;
    shaderCode->size = 0;
    free(shaderCode->data);
    free(shaderCode);
}

ShaderIntermediate *compileShaderCode(ShaderCode *shaderCode) {
    shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode->data, shaderCode->size, shaderCode->stage, "shader", "main", shaderCompileOptions);
    shaderc_compilation_status   status = shaderc_result_get_compilation_status(result);

    if(status != shaderc_compilation_status_success) {
        debug("%s", shaderc_result_get_error_message(result));
        shaderc_result_release(result);
        assert(status == shaderc_compilation_status_success);
    }

    ShaderIntermediate *shaderIntermediate = malloc(sizeof(ShaderIntermediate));
    shaderIntermediate->stage = shaderCode->stage;
    shaderIntermediate->size  = shaderc_result_get_length(result);
    shaderIntermediate->data  = malloc(shaderIntermediate->size);
    memcpy(shaderIntermediate->data, shaderc_result_get_bytes(result), shaderIntermediate->size);

    shaderc_result_release(result);

    debug("%s shader code with %lu bytes of size compiled successfully", shaderIntermediate->stage == shaderc_vertex_shader ? "Vertex" : "Fragment", shaderIntermediate->size);

    return shaderIntermediate;
}

void freeShaderIntermediate(ShaderIntermediate *shaderIntermediate) {
    shaderIntermediate->stage = shaderc_glsl_infer_from_source;
    shaderIntermediate->size = 0;
    free(shaderIntermediate->data);
    free(shaderIntermediate);
}

ShaderModule *createShaderModule(ShaderIntermediate *shaderIntermediate) {
    ShaderModule *shaderModule = malloc(sizeof(ShaderModule));

    if(shaderIntermediate->stage == shaderc_vertex_shader) {
        shaderModule->stage     = VK_SHADER_STAGE_VERTEX_BIT;
        shaderModule->nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if(shaderIntermediate->stage == shaderc_fragment_shader) {
        shaderModule->stage     = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderModule->nextStage = 0;
    } // TODO: Add other shader types

    VkDescriptorSetLayout layouts[] = {
        sceneDescriptorPool.layout,
        primitiveDescriptorPool.layout,
        materialDescriptorPool.layout
    };

    VkShaderCreateInfoEXT shaderCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT,
        .stage = shaderModule->stage,
        .nextStage = shaderModule->nextStage,
        .codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .codeSize = shaderIntermediate->size,
        .pCode = shaderIntermediate->data,
        .pName = "main",
        .setLayoutCount = sizeof(layouts) / sizeof(VkDescriptorSetLayout),
        .pSetLayouts = layouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
        .pSpecializationInfo = NULL
    };

    PFN_vkCreateShadersEXT createShaders = loadDeviceFunction("vkCreateShadersEXT");
    createShaders(device, 1, &shaderCreateInfo, NULL, &shaderModule->module);
    debug("%s shader module created successfully", shaderModule->stage == VK_SHADER_STAGE_VERTEX_BIT ? "Vertex" : "Fragment");

    return shaderModule;
}

void destroyShaderModule(ShaderModule *shaderModule) {
    shaderModule->stage = VK_SHADER_STAGE_ALL;
    shaderModule->nextStage = VK_SHADER_STAGE_ALL;

    PFN_vkDestroyShaderEXT destroyShader = loadDeviceFunction("vkDestroyShaderEXT");
    destroyShader(device, shaderModule->module, NULL);

    free(shaderModule);
}

void createModules() {
    shaderCompiler = shaderc_compiler_initialize();
    assert(shaderCompiler);

    shaderCompileOptions = shaderc_compile_options_initialize();
    assert(shaderCompileOptions);

    shaderc_compile_options_set_source_language(shaderCompileOptions, shaderc_source_language_glsl);
    shaderc_compile_options_set_target_spirv(shaderCompileOptions, shaderc_spirv_version_1_6);
    shaderc_compile_options_set_target_env(shaderCompileOptions, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
#if DEBUG
    shaderc_compile_options_set_generate_debug_info(shaderCompileOptions);
#else
    shaderc_compile_options_set_optimization_level(shaderCompileOptions, shaderc_optimization_level_performance);
#endif

    debug("Shader compiler created and compile options set");

    ShaderCode *vertexShaderCode   = loadShaderCode("shaders", "vertex.vert",   shaderc_vertex_shader);
    ShaderCode *fragmentShaderCode = loadShaderCode("shaders", "fragment.frag", shaderc_fragment_shader);

    ShaderIntermediate *vertexShaderIntermediate   = compileShaderCode(vertexShaderCode);
    ShaderIntermediate *fragmentShaderIntermediate = compileShaderCode(fragmentShaderCode);

    vertexShaderModule   = createShaderModule(vertexShaderIntermediate);
    fragmentShaderModule = createShaderModule(fragmentShaderIntermediate);

    debug("Shader modules created");
}

void bindShaders(VkCommandBuffer commandBuffer, ShaderModule *vertex, ShaderModule *fragment) {
    assert(vertex->stage == VK_SHADER_STAGE_VERTEX_BIT && fragment->stage == VK_SHADER_STAGE_FRAGMENT_BIT);

    VkShaderStageFlagBits stages[] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT
    };

    VkShaderEXT modules[] = {
        vertex->module,
        fragment->module
    };

    uint32_t stageCount = sizeof(stages) / sizeof(VkShaderStageFlags);

    PFN_vkCmdBindShadersEXT cmdBindShaders = loadDeviceFunction("vkCmdBindShadersEXT");
    cmdBindShaders(commandBuffer, stageCount, stages, modules);
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    debug("Shader compiler and shader compile options released");

    destroyShaderModule(vertexShaderModule);
    destroyShaderModule(fragmentShaderModule);

    debug("Shader modules destroyed");
}
