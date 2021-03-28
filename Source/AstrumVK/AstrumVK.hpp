#ifndef __AstrumVK_AstrumVK_hpp__
#define __AstrumVK_AstrumVK_hpp__

#pragma once

#include "Core/Window.hpp"

class AstrumVK
{
private:
    VkInstance instance;
    void createInstance();
    void destroyInstance();

    Window& window;

    AstrumVK(const AstrumVK&) = delete;
    AstrumVK& operator=(const AstrumVK&) = delete;

public:
    AstrumVK(Window& _window);
    ~AstrumVK();
};

#endif