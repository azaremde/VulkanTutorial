#ifndef __AstrumVK_Surface_Surface_hpp__
#define __AstrumVK_Surface_Surface_hpp__

#pragma once

#include "Pch.hpp"

#include "Core/Window.hpp"

class Surface
{
private:
    VkSurfaceKHR surface;

    VkInstance& instance;
    Window& window;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

public:
    const VkSurfaceKHR& getSurface() const;

    Surface(VkInstance& _instance, Window& _window);
    ~Surface();
};

#endif