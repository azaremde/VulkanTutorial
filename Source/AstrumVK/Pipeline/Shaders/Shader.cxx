#include "Shader.hpp"

VkShaderModule Shader::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule;

    VK_CHECK(
        vkCreateShaderModule(GPU::getDevice(), &createInfo, nullptr, &shaderModule),
        "Failed to create shader module."
    );

    return shaderModule;
}

Shader::Shader(const std::string& vertShaderPath, const std::string& fragShaderPath)
{
    std::vector<char> vertShaderCode = ShadersUtil::readFile(vertShaderPath);
    std::vector<char> fragShaderCode = ShadersUtil::readFile(fragShaderPath);

    assert(
        (vertShaderCode.size() % 4 == 0) && 
        "Code size must be multiple of 4. You must have loaded an uncompiled shader in the ASCII format."
    );

    assert(
        (fragShaderCode.size() % 4 == 0) && 
        "Code size must be multiple of 4. You must have loaded an uncompiled shader in the ASCII format."
    );

    vertShaderModule = createShaderModule(vertShaderCode);
    fragShaderModule = createShaderModule(fragShaderCode);

    DebugLogOut("Shaders created.");
}

Shader::~Shader()
{
    vkDestroyShaderModule(GPU::getDevice(), vertShaderModule, nullptr);
    vkDestroyShaderModule(GPU::getDevice(), fragShaderModule, nullptr);

    DebugLogOut("Shaders destroyed.");
}

const VkShaderModule& Shader::getVertShaderModule() const
{
    return vertShaderModule;
}

const VkShaderModule& Shader::getFragShaderModule() const
{
    return fragShaderModule;
}

namespace ShadersUtil
{
    
std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

}