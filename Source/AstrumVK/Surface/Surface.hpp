#ifndef __AstrumVK_Surface_Surface_hpp__
#define __AstrumVK_Surface_Surface_hpp__

#pragma once

#include "Pch.hpp"

#include "Core/Window.hpp"

#include "AstrumVK/Instance/Instance.hpp"

class Surface
{
private:
    inline static VkSurfaceKHR surface;

    inline static Window* window;

public:
    static void create(Window* _window);
    static void destroy();

    static const VkSurfaceKHR& getSurface();
};

#endif