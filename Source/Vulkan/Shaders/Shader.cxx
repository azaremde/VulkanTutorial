#include "Shader.hpp"

Shader::Shader(GPU& _gpu, const std::string& vertShaderPath, const std::string& fragShaderPath) : gpu { _gpu }
{
    auto vertShaderCode = ShaderUtil::ReadFile(vertShaderPath);
    auto fragShaderCode = ShaderUtil::ReadFile(fragShaderPath);

    vertShaderModule = ShaderUtil::CreateShaderModule(gpu.Device(), vertShaderCode);
    fragShaderModule = ShaderUtil::CreateShaderModule(gpu.Device(), fragShaderCode);
}

Shader::~Shader()
{
    vkDestroyShaderModule(gpu.Device(), vertShaderModule, nullptr);
    vkDestroyShaderModule(gpu.Device(), fragShaderModule, nullptr);
}

VkShaderModule& Shader::GetVertexShaderModule()
{
    return vertShaderModule;
}

VkShaderModule& Shader::GetFragmentShaderModule()
{
    return fragShaderModule;
}

void Shader::RetrieveCreateInfoStruct(std::vector<VkPipelineShaderStageCreateInfo>& result)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    result.resize(2);
    result[0] = vertShaderStageInfo;
    result[1] = fragShaderStageInfo;
}

namespace ShaderUtil
{
    std::vector<char> ReadFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        DebugLogOut("Loading file: " << filename);
        DebugLogOut("Size: " << fileSize);

        return buffer;
    }

    VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        VulkanCheck(
            vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule),
            "Failed to create shader module."
        );

        return shaderModule;
    }
}