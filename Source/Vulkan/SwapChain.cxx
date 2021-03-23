#include "SwapChain.hpp"

SwapChain::SwapChain(VkInstance& _instance, Window& _window) : instance{ _instance }, window{ _window }
{
	//CreateSurface();
}

//void SwapChain::CreateSurface()
//{
//	VulkanCheck(
//		glfwCreateWindowSurface(instance, window.GetGlfwWindow(), nullptr, &surface),
//		"Failed to create window surface."
//	);
//}

SwapChain::~SwapChain()
{
	//vkDestroySurfaceKHR(instance, surface, nullptr);
}