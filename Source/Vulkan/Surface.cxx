#include "Surface.hpp"

Surface::Surface(VkInstance& _instance, Window& _window) : instance{ _instance }, window{ _window }
{
	VulkanCheck(
		glfwCreateWindowSurface(instance, window.GetGlfwWindow(), nullptr, &surface),
		"Failed to create surface"
	);
}

Surface::~Surface()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}

VkSurfaceKHR& Surface::GetSurface()
{
	return surface;
}