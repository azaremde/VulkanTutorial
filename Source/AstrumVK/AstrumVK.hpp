#ifndef __AstrumVK_AstrumVK_hpp__
#define __AstrumVK_AstrumVK_hpp__

#pragma once

#include "Core/Window.hpp"

#include "GPU/GPU.hpp"
#include "Surface/Surface.hpp"
#include "SwapChain/SwapChain.hpp"
#include "Pipeline/Pipeline.hpp"
#include "Pipeline/Shaders/Shader.hpp"
#include "GPU/CommandBuffer.hpp"
#include "UBO/UniformBuffer.hpp"
#include "Images/Image.hpp"
#include "Time.hpp"

class AstrumVK : public IOnViewportResize
{
private:
    VkInstance instance;
    void createInstance();
    void destroyInstance();

    void createDebugger();
    void destroyDebugger();

    Surface* surface;
    void createSurface();
    void destroySurface();

    GPU* gpu;
    void createGPU();
    void destroyGPU();

    // Consider making screen-framebuffers a field of AstrumVK. (Make it independent from SwapChain).
    SwapChain* swapChain;
    void createSwapChain();
    void destroySwapChain();

    Shader* defaultShader;
    Pipeline* pipeline;
    void createPipeline();
    void destroyPipeline();

    UniformBuffer* uniformBuffer;
    void createUniformBuffer();
    void destroyUniformBuffer();

    void createSwapChainFramebuffers();
    void destroySwapChainFramebuffers();

    CommandBuffer* commandBuffer;
    void createCommandBuffer();
    void destroyCommandBuffer();

    Image* image_0;
    Image* image_1;
    void createImage();
    void destroyImage();

    DynamicUBO* ubos = new DynamicUBO[2];
    DynamicUBO* getUbo(uint32_t index);

    StaticUBO staticUbo;

    std::vector<VAO*> renderList;

    Time time;

    Window& window;

    AstrumVK(const AstrumVK&) = delete;
    AstrumVK& operator=(const AstrumVK&) = delete;

public:
    void onViewportResize(unsigned int newWidth, unsigned int newHeight) override;

    void drawFrame();

    void awaitDeviceIdle();

    AstrumVK(Window& _window);
    ~AstrumVK();
};

#endif