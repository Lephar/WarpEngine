#include "shader.h"

#include "helper.h"
#include "file.h"
#include "buffer.h"
#include "content.h"

shaderc_compiler_t shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

extern VkDevice device;

ShaderModule   vertexShader;
ShaderModule fragmentShader;

ShaderCode loadShaderCode(const char *path, FileType type, shaderc_shader_kind stage) {
    ShaderCode shaderCode = {
        .type = type,
        .stage = stage,
        .data = readFile(path, type)
    };

    return shaderCode;
}

ShaderCode compileShaderCode(ShaderCode shaderCode) {
    assert(shaderCode.type == FILE_TYPE_TEXT);

    shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode.data.content, shaderCode.data.size - 1, shaderCode.stage, "shader", "main", shaderCompileOptions);
    shaderc_compilation_status status = shaderc_result_get_compilation_status(result);

    if(status != shaderc_compilation_status_success) {
        debug("%s", shaderc_result_get_error_message(result));
        shaderc_result_release(result);
    }

    assert(status == shaderc_compilation_status_success);
    debug("\tSuccessfully compiled");

    ShaderCode compiledShaderCode = {
        .type = FILE_TYPE_BINARY,
        .stage = shaderCode.stage,
        .data = {
            .size = shaderc_result_get_length(result),
            .content = malloc(compiledShaderCode.data.size)
        }
    };

    memcpy(compiledShaderCode.data.content, shaderc_result_get_bytes(result), compiledShaderCode.data.size);
    debug("\tCompiled size: %ld", compiledShaderCode.data.size);

    shaderc_result_release(result);
}

ShaderModule createModule(ShaderCode shaderCode) {
    debug("Shader: %s", shaderCode->name);

    const char *extension = ".glsl";

    shaderc_shader_kind shaderKind = 0;
    VkShaderStageFlags nextStage = 0;

    if(shaderCode->stage == VK_SHADER_STAGE_COMPUTE_BIT) {
        extension = ".comp";
        shaderKind = shaderc_compute_shader;
    } else if(shaderCode->stage == VK_SHADER_STAGE_VERTEX_BIT) {
        extension = ".vert";
        shaderKind = shaderc_vertex_shader;
        nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if(shaderCode->stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
        extension = ".frag";
        shaderKind = shaderc_fragment_shader;
    } //TODO: Add other shader types

    char shaderFile[PATH_MAX];
    sprintf(shaderFile, "%s%s%s", shaderCode->name, extension, shaderCode->intermediate ? ".spv" : "");
    debug("\tPath:          %s", shaderFile);

    if(shaderCode->intermediate) {
        readFile(shaderFile, 1, &shaderCode->size, &shaderCode->data);
        debug("\tSize:          %ld", shaderCode->size);
    } else {

    }

    VkShaderCreateInfoEXT shaderCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT,
        .stage = shader->stage,
        .nextStage = nextStage,
        .codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .codeSize = shader->size,
        .pCode = shader->data,
        .pName = "main",
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
        .pSpecializationInfo = NULL
    };

    PFN_vkCreateShadersEXT createShaders = loadFunction("vkCreateShadersEXT");
    assert(createShaders);

    createShaders(device, 1, &shaderCreateInfo, NULL, &shaderCode->module);
    debug("\tSuccessfully created");
}

void createModules() {
    shaderCompiler = shaderc_compiler_initialize();

    shaderCompileOptions = shaderc_compile_options_initialize();
#ifdef DEBUG
    shaderc_compile_options_set_optimization_level(shaderCompileOptions, shaderc_optimization_level_performance);
#endif // DEBUG

    debug("Shader compiler and shader compile options set");

    debug("Shader count: %d", shaderCount);

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        createModule(shaderReferences[shaderIndex]);
    }

    debug("Shader modules created");
}

void destroyModule(Shader *shader) {
    PFN_vkDestroyShaderEXT destroyShader = loadFunction("vkDestroyShaderEXT");
    assert(destroyShader);
    destroyShader(device, shader->module, NULL);

    free(shader->data);

    debug("Shader module %s destroyed", shader->name);
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    debug("Shader compiler and shader compile options released");

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        destroyModule(shaderReferences[shaderIndex]);
    }

    debug("Shader modules destroyed");
}
