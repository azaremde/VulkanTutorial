#include "Surface.hpp"

Surface::Surface(VkInstance& _instance, Window& _window) : instance { _instance }, window { _window }
{
    VK_CHECK(
        glfwCreateWindowSurface(instance, window.getGlfwWindow(), nullptr, &surface),
        "Failed to create surface"
    );

    DebugLogOut("Surface successfully created.");
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);

    DebugLogOut("Surface destroyed.");
}

const VkSurfaceKHR& Surface::getSurface() const
{
    return surface;
}