#ifndef __AstrumVK_Pipeline_Pipeline_hpp__
#define __AstrumVK_Pipeline_Pipeline_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"

#include "Shaders/Shader.hpp"
#include "RenderPasses/RenderPass.hpp"

class Pipeline
{
private:
    // TODO: Substitute everything in a struct.
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    void setShaderStages();

    struct Fixed
    {
        VkVertexInputBindingDescription bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        void setVertexInputInfo();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        void setInputAssemblyInfo();

        VkViewport viewport{};
        void setViewportInfo(const VkExtent2D& extent);

        VkRect2D scissor{};
        void setScissorInfo(const VkExtent2D& extent);

        VkPipelineViewportStateCreateInfo viewportState{};
        void setViewportState();

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        void setRasterizerState();

        VkPipelineMultisampleStateCreateInfo multisampling{};
        void setMultisamplingState();

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        void setColorBlendAttachment();

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        void setColorBlending();

        VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        void setDynamicStates();
    } fixed;

    VkPipelineLayout pipelineLayout{};
    void createPipelineLayout();
    void destroyPipelineLayout();

    RenderPass* renderPass;
    void createRenderPass();
    void destroyRenderPass();

    VkPipeline pipeline;
    void createPipeline();
    void destroyPipeline();

    GPU& gpu;
    SwapChain& swapChain;
    Shader& shader;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

public:
    void createGraphicsPipeline();
    void destroyGraphicsPipeline();

    const VkPipeline& getPipeline() const;
    const VkRenderPass& getRenderPass() const;

    Pipeline(GPU& _gpu, SwapChain& _swapChain, Shader& _shader);
    ~Pipeline();
};

#endif