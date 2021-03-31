#include "AstrumVK.hpp"

#include "Debug.hpp"

// Todo: move to its own class.
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

void AstrumVK::createCommandBuffer()
{
    commandBuffer = new CommandBuffer(*gpu, *swapChain);
}

void AstrumVK::destroyCommandBuffer()
{
    delete commandBuffer;
}

AstrumVK::AstrumVK(Window& _window) : window { _window }
{
    window.addOnViewportResizeSubscriber(this);

    createInstance();
    createDebugger();
    createSurface();
    createGPU();
    createSwapChain();
    createPipeline();
    createSwapChainFramebuffers();
    createCommandBuffer();

    VAO* vao1 = new VAO();
    VAO* vao2 = new VAO();
    
    commandBuffer->createVertexBuffer(vao1, {
        {{-0.2f - 0.3f, -0.2f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f - 0.3f, -0.2f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.2f - 0.3f,  0.2f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f - 0.3f,  0.2f}, {1.0f, 1.0f, 1.0f}}
    });

    commandBuffer->createIndexBuffer(vao1, std::vector<uint32_t> {
        0, 1, 2, 2, 3, 0
    });
    
    commandBuffer->createVertexBuffer(vao2, {
        {{-0.2f + 0.3f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f + 0.3f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.2f + 0.3f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f + 0.3f,  0.5f}, {1.0f, 1.0f, 1.0f}}
    });

    commandBuffer->createIndexBuffer(vao2, {
        0, 1, 2, 2, 3, 0
    });
    
    renderList.emplace_back(vao1);
    renderList.emplace_back(vao2);

    commandBuffer->render(
        pipeline->getRenderPass(),
        swapChain->getFramebuffers(),
        swapChain->getExtent(),
        pipeline->getPipeline(),
        {}
    );
}

AstrumVK::~AstrumVK()
{
    for (auto& vao : renderList)
    {
        vao->destroy(*gpu);
        delete vao;
    }

    destroyCommandBuffer();
    destroySwapChainFramebuffers();
    destroyPipeline();
    destroySwapChain();
    destroyGPU();
    destroySurface();
    destroyDebugger();
    destroyInstance();
}

void AstrumVK::drawFrame()
{
    time.beginFrame();

    static float timer { 0.0f };

    timer += time.getDeltaTime();

    if (timer >= 1.0f)
    {
        DebugLogOut(time.getFps());

        timer = 0.0f;
    }

    if (glfwGetKey(window.getGlfwWindow(), GLFW_KEY_SPACE))
    {
        awaitDeviceIdle();

        commandBuffer->render(
            pipeline->getRenderPass(),
            swapChain->getFramebuffers(),
            swapChain->getExtent(),
            pipeline->getPipeline(),
            renderList
        );
    }

    swapChain->acquireImage();
    swapChain->submit(commandBuffer->getCommandBuffers());
    swapChain->present();

    time.endFrame();
}

void AstrumVK::awaitDeviceIdle()
{
    vkDeviceWaitIdle(gpu->getDevice());
}

void AstrumVK::onViewportResize(unsigned int newWidth, unsigned int newHeight)
{
    awaitDeviceIdle();

    gpu->recheckDeviceCapabilities();

    commandBuffer->freeCommandBuffers();
    swapChain->destroyFramebuffers();
    pipeline->destroyGraphicsPipeline();
    swapChain->destroySwapChain();

    swapChain->createSwapChain();
    pipeline->createGraphicsPipeline();
    swapChain->createFramebuffers(pipeline->getRenderPass());
    commandBuffer->createCommandBuffers();

    commandBuffer->render(
        pipeline->getRenderPass(), 
        swapChain->getFramebuffers(), 
        swapChain->getExtent(), 
        pipeline->getPipeline(), 
        renderList
    );
}