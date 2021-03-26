#include "Vulkan.hpp"

#include "Debug.hpp"

Vulkan::Vulkan(Window& _window) : window{ _window }
{
	window.AddOnWindowResizeSubscriber(this);

	CreateInstance();
	CreateDebugger();
	CreateSurface();
	CreateGPU();
	CreateSwapChain();
	CreatePipeline();
	CreateDrawing();
}

void Vulkan::CreateInstance()
{
	if (Debug::validationLayersEnabled && !Debug::CheckValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested but not suppored.");
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = "Vulkan App";
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No engine";

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (Debug::validationLayersEnabled)
	{
		Debug::PopulateCreateInfoStruct(debugCreateInfo);

		createInfo.enabledLayerCount = static_cast<uint32_t>(Debug::validationLayers.size());
		createInfo.ppEnabledLayerNames = Debug::validationLayers.data();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.pApplicationInfo = &applicationInfo;

	VulkanCheck(
		vkCreateInstance(&createInfo, nullptr, &instance),
		"Failed to create vulkan instance."
	);
}

void Vulkan::DestroyInstance()
{
	vkDestroyInstance(instance, nullptr);
}

void Vulkan::CreateDebugger()
{
	Debug::CreateMessenger(instance);
}

void Vulkan::DestroyDebugger()
{
	Debug::DestroyMessenger(instance);
}

std::vector<const char*> Vulkan::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount{ 0 };
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (Debug::validationLayersEnabled)
	{
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void Vulkan::CreateSurface()
{
	surface = new Surface(instance, window);
}

void Vulkan::DestroySurface()
{
	delete surface;
}

void Vulkan::CreateGPU()
{
	gpu = new GPU(instance, *surface);
}

void Vulkan::DestroyGPU()
{
	delete gpu;
}

void Vulkan::CreateSwapChain()
{
	swapChain = new SwapChain(instance, *gpu, *surface, window);
}

void Vulkan::DestroySwapChain()
{
	delete swapChain;
}

void Vulkan::CreatePipeline()
{
	pipeline = new Pipeline(*gpu, *swapChain);
}

void Vulkan::DestroyPipeline()
{
	delete pipeline;
}

void Vulkan::CreateDrawing()
{
	drawing = new Drawing(*gpu, pipeline->GetOutputFramebuffer(), *pipeline, *swapChain);
}

void Vulkan::DestroyDrawing()
{
	delete drawing;
}

void Vulkan::OnViewportResize(int newWidth, int newHeight)
{
	drawing->WaitForIdle();

	gpu->CheckDeviceProperties();

	swapChain->Destroy();
	swapChain = new SwapChain(instance, *gpu, *surface, window);

	pipeline->Destroy();
	pipeline = new Pipeline(*gpu, *swapChain);

	drawing->Destroy();
	drawing = new Drawing(*gpu, pipeline->GetOutputFramebuffer(), *pipeline, *swapChain);
}

void Vulkan::Draw()
{
	drawing->Draw();
}

void Vulkan::WaitForIdle()
{
	drawing->WaitForIdle();
}

Vulkan::~Vulkan()
{
	DestroyDrawing();
	DestroyPipeline();
	DestroySwapChain();
	DestroyGPU();
	DestroySurface();
	DestroyDebugger();
	DestroyInstance();
}
