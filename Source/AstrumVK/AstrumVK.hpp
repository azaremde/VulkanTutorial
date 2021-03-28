#ifndef __AstrumVK_AstrumVK_hpp__
#define __AstrumVK_AstrumVK_hpp__

#pragma once

#include "Core/Window.hpp"

#include "GPU/GPU.hpp"
#include "Surface/Surface.hpp"

class AstrumVK
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

    Window& window;

    AstrumVK(const AstrumVK&) = delete;
    AstrumVK& operator=(const AstrumVK&) = delete;

public:
    AstrumVK(Window& _window);
    ~AstrumVK();
};

#endif