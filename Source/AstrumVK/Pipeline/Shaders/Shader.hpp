#ifndef __AstrumVK_Pipeline_Shaders_Shader_hpp__
#define __AstrumVK_Pipeline_Shaders_Shader_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"

class Shader
{
private:
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkShaderModule createShaderModule(const std::vector<char>& code);

    GPU& gpu;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

public:
    Shader(GPU& _gpu, const std::string& vertShaderPath, const std::string& fragShaderPath);
    ~Shader();

    const VkShaderModule& getVertShaderModule() const;
    const VkShaderModule& getFragShaderModule() const;
};

namespace ShadersUtil {

static std::vector<char> readFile(const std::string& filename);

}

#endif