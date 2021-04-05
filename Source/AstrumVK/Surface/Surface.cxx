#include "Surface.hpp"

void Surface::create(Window* _window)
{
    window = _window;

    VK_CHECK(
        glfwCreateWindowSurface(Instance::getInstance(), window->getGlfwWindow(), nullptr, &surface),
        "Failed to create surface"
    );

    DebugLogOut("Surface successfully created.");
}

void Surface::destroy()
{    
    vkDestroySurfaceKHR(Instance::getInstance(), surface, nullptr);

    DebugLogOut("Surface destroyed.");
}

const VkSurfaceKHR& Surface::getSurface()
{
    return surface;
}