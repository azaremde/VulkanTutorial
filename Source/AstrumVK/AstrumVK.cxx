#include "AstrumVK.hpp"

#include "Debug.hpp"

#include "Models/Model.hpp"

// Wrapper functions for aligned memory allocation
// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
void* alignedAlloc(size_t size, size_t alignment)
{
	void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	data = _aligned_malloc(size, alignment);
#else
	int res = posix_memalign(&data, alignment, size);
	if (res != 0)
		data = nullptr;
#endif
	return data;
}



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
    defaultShader = new Shader(*gpu, "Assets/Shaders/DefaultShader.vert.spv", "Assets/Shaders/DefaultShader.frag.spv");

    pipeline = new Pipeline(*gpu, *swapChain, *defaultShader);
}

void AstrumVK::destroyPipeline()
{
    delete pipeline;

    delete defaultShader;
}

void AstrumVK::createUniformBuffer()
{
    UniformLayout dynamicLayout{};
    UniformLayout staticLayout{};
    UniformLayout imageLayout{};

    dynamicLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    dynamicLayout.binding = 0;
    dynamicLayout.size = sizeof(DynamicUBO);
    dynamicLayout.instances = 2;

    staticLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    staticLayout.binding = 1;
    staticLayout.size = sizeof(StaticUBO);

    imageLayout.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageLayout.binding = 2;
    imageLayout.size = sizeof(uint32_t);
    
    imageLayout.imageView_0 = image_0->getImageView();
    imageLayout.sampler_0 = image_0->getSampler();
    
    imageLayout.imageView_1 = image_1->getImageView();
    imageLayout.sampler_1 = image_1->getSampler();

    uniformBuffer = new UniformBuffer(
        *gpu, 
        *swapChain, 
        *pipeline,
        { dynamicLayout, staticLayout, imageLayout }
    );
}

void AstrumVK::destroyUniformBuffer()
{
    delete uniformBuffer;
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
    commandBuffer = new CommandBuffer(*gpu, *swapChain, *pipeline);
}

void AstrumVK::destroyCommandBuffer()
{
    delete commandBuffer;
}

void AstrumVK::createImage()
{
    image_0 = new Image(*gpu, *commandBuffer, "Assets/Images/texture.jpg");
    image_1 = new Image(*gpu, *commandBuffer, "Assets/Images/diffuse.png");
}

void AstrumVK::destroyImage()
{
    delete image_0;
    delete image_1;
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
    createImage();
    createUniformBuffer();

    VAO* vao1 = new VAO();
    VAO* vao2 = new VAO();

    Model* cubeModel = loadModel("Assets/Models/cube.obj");
    Model* sphereModel = loadModel("Assets/Models/base.fbx");

    commandBuffer->createVertexBuffer(vao1, cubeModel->vertices);
    commandBuffer->createIndexBuffer(vao1, cubeModel->indices);

    commandBuffer->createVertexBuffer(vao2, sphereModel->vertices);
    commandBuffer->createIndexBuffer(vao2, sphereModel->indices);
    
    renderList.emplace_back(vao1);
    renderList.emplace_back(vao2);

    commandBuffer->render(
        pipeline->getRenderPass(),
        swapChain->getFramebuffers(),
        swapChain->getExtent(),
        pipeline->getPipeline(),
        *uniformBuffer,
        renderList
    );

    ubos = (DynamicUBO*)alignedAlloc(uniformBuffer->layouts[0].bufferSize, uniformBuffer->layouts[0].dynamicAlignment);

    getUbo(0)->model = glm::mat4x4(1);
    getUbo(1)->model = glm::mat4x4(1);

    staticUbo.proj = glm::perspective(
        glm::radians(70.0f), 
        static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight()),
        0.1f, 
        1000.0f);

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++)
    {
        uniformBuffer->updateUniformBuffer(i, 1, sizeof(StaticUBO), &staticUbo);
    }
}

DynamicUBO* AstrumVK::getUbo(uint32_t index)
{
    return (DynamicUBO*)(((uint64_t)ubos + (index * uniformBuffer->layouts[0].dynamicAlignment)));
}

AstrumVK::~AstrumVK()
{
    for (auto& vao : renderList)
    {
        vao->destroy(*gpu);
        delete vao;
    }

    destroyUniformBuffer();
    destroyImage();
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

    static float t { 0.0f };

    t += time.getDeltaTime() * 100.0f;

    glm::mat4x4& model_0 = getUbo(0)->model;
    glm::mat4x4& model_1 = getUbo(1)->model;

    model_0 = glm::mat4x4(1);
    model_0 = glm::translate(model_0, glm::vec3(0, 0, -10));
    model_0 = glm::rotate(model_0, glm::radians(t), glm::vec3(1, 0, 0));
    model_0 = glm::rotate(model_0, glm::radians(t), glm::vec3(0, 1, 0));
    model_0 = glm::rotate(model_0, glm::radians(t), glm::vec3(0, 0, 1));

    model_1 = glm::mat4x4(1);
    model_1 = glm::translate(model_1, glm::vec3(3, 0, -20));
    model_1 = glm::rotate(model_1, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model_1 = glm::rotate(model_1, glm::radians(t * 1.0f), glm::vec3(0, 0, 1));
    model_1 = glm::scale(model_1, glm::vec3(0.01f));

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
            *uniformBuffer,
            renderList
        );
    }

    swapChain->acquireImage();

    uniformBuffer->updateUniformBuffer(swapChain->getImageIndex(), 0, uniformBuffer->layouts[0].dynamicAlignment * uniformBuffer->layouts[0].instances, ubos);

    swapChain->syncImagesInFlight();
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
        *uniformBuffer,
        renderList
    );

    staticUbo.proj = glm::perspective(
        glm::radians(70.0f), 
        static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight()),
        0.1f, 
        1000.0f
    );

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++)
    {
        uniformBuffer->updateUniformBuffer(i, 1, sizeof(StaticUBO), &staticUbo);
    }
}