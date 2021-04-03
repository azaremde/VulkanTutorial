#include "Surface.hpp"

void Surface::create(VkInstance* _instance, Window* _window)
{
    instance = _instance;
    window = _window;

    VK_CHECK(
        glfwCreateWindowSurface(*instance, window->getGlfwWindow(), nullptr, &surface),
        "Failed to create surface"
    );

    DebugLogOut("Surface successfully created.");
}

void Surface::destroy()
{    
    vkDestroySurfaceKHR(*instance, surface, nullptr);

    DebugLogOut("Surface destroyed.");
}

const VkSurfaceKHR& Surface::getSurface()
{
    return surface;
}