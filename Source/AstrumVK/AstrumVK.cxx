#include "AstrumVK.hpp"

#include "Debug.hpp"

void AstrumVK::createInstance()
{
    if (Debug::validationLayersEnabled && !Debug::checkValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers required but not supported.");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = window.getTitle().c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "No engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    auto glfwExtensions = Debug::getRequiredExtensions();
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
    createInfo.ppEnabledExtensionNames = glfwExtensions.data();

    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (Debug::validationLayersEnabled)
    {
        Debug::populateCreateInfoStruct(debugCreateInfo);

        createInfo.enabledLayerCount = static_cast<uint32_t>(Debug::validationLayers.size());
        createInfo.ppEnabledLayerNames = Debug::validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VK_CHECK(
        vkCreateInstance(&createInfo, nullptr, &instance),
        "Failed to create instance."
    );

    DebugLogOut("Instance created.");
}

void AstrumVK::destroyInstance()
{
    vkDestroyInstance(instance, nullptr);
    
    DebugLogOut("Instance destroyed.");
}

void AstrumVK::createDebugger()
{
    Debug::createMessenger(instance);
}

void AstrumVK::destroyDebugger()
{
    Debug::destroyMessenger(instance);
}

void AstrumVK::createSurface()
{
    surface = new Surface(instance, window);
}

void AstrumVK::destroySurface()
{
    delete surface;
}

void AstrumVK::createGPU()
{
    gpu = new GPU(instance, *surface);
}

void AstrumVK::destroyGPU()
{
    delete gpu;
}

void AstrumVK::createSwapChain()
{
    swapChain = new SwapChain(*gpu, *surface, window);
}

void AstrumVK::destroySwapChain()
{
    delete swapChain;
}

void AstrumVK::createPipeline()
{
    defaultShader = new Shader(*gpu, "Shaders/DefaultShader.vert.spv", "Shaders/DefaultShader.frag.spv");

    pipeline = new Pipeline(*gpu, *swapChain, *defaultShader);
}

void AstrumVK::destroyPipeline()
{
    delete pipeline;

    delete defaultShader;
}

void AstrumVK::createSwapChainFramebuffers()
{
    swapChain->createFramebuffers(pipeline->getRenderPass());
}

void AstrumVK::destroySwapChainFramebuffers()
{
    swapChain->destroyFramebuffers();
}

AstrumVK::AstrumVK(Window& _window) : window { _window }
{
    createInstance();
    createDebugger();
    createSurface();
    createGPU();
    createSwapChain();
    createPipeline();
    createSwapChainFramebuffers();
}

AstrumVK::~AstrumVK()
{
    destroySwapChainFramebuffers();
    destroyPipeline();
    destroySwapChain();
    destroyGPU();
    destroySurface();
    destroyDebugger();
    destroyInstance();
}