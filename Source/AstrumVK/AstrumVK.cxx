#include "AstrumVK.hpp"

#include "Debug.hpp"

#include "Assets/MeshAsset.hpp"
#include "Assets/ImageAsset.hpp"

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
    // gpu = new GPU(instance, *surface);
    GPU::create(&instance, surface);
}

void AstrumVK::destroyGPU()
{
    // delete gpu;
    GPU::destroy();
}

void AstrumVK::createSwapChain()
{
    swapChain = new SwapChain(*surface, window, true);
}

void AstrumVK::destroySwapChain()
{
    delete swapChain;
}

void AstrumVK::createPipeline()
{
    defaultShader = new Shader("Assets/Shaders/DefaultShader.vert.spv", "Assets/Shaders/DefaultShader.frag.spv");

    pipeline = new Pipeline(*swapChain, *defaultShader);
}

void AstrumVK::destroyPipeline()
{
    delete pipeline;

    delete defaultShader;
}

void AstrumVK::createModels(const std::vector<ModelDescriptor>& models)
{
    UniformLayout dynamicLayout{};
    UniformLayout staticLayout{};
    UniformLayout imageLayout{};

    dynamicLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    dynamicLayout.binding = 0;
    dynamicLayout.size = sizeof(DynamicUBO);
    dynamicLayout.instances = static_cast<uint32_t>(models.size());

    staticLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    staticLayout.binding = 1;
    staticLayout.size = sizeof(StaticUBO);

    imageLayout.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageLayout.binding = 2;
    imageLayout.size = sizeof(uint32_t);

    for (int i = 0; i < models.size(); i++)
    {
        MeshAsset* model = loadModelAsset(models[i].modelFilename);
        ImageAsset* image = loadImageAsset(models[i].textureFilename);

        Texture2D* img = new Texture2D(*commandBuffer, image);

        Entity* entity = new Entity();
        commandBuffer->createVertexBuffer(entity, model->vertices);
        commandBuffer->createIndexBuffer(entity, model->indices);
        entity->albedoColor = img;

        imageLayout.imageViews.emplace_back(img->getImageView());
        imageLayout.samplers.emplace_back(img->getSampler());

        renderList.emplace_back(entity);

        delete model;
        delete image;
    }

    uniformBuffer = new UniformBuffer(
        *swapChain, 
        *pipeline,
        { dynamicLayout, staticLayout, imageLayout },
        renderList
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
    commandBuffer = new CommandBuffer(*swapChain, *pipeline);
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

    window.setTitle(GPU::about.name);

    createSwapChain();
    createPipeline();
    createSwapChainFramebuffers();
    createCommandBuffer();

    createModels({
        { "Assets/Models/cube.obj", "Assets/Images/texture.jpg" },
        { "Assets/Models/base.fbx", "Assets/Images/diffuse.png" },
        { "Assets/Models/base.fbx", "Assets/Images/diffuse.png" }
    });

    commandBuffer->render(
        pipeline->getRenderPass(),
        swapChain->getFramebuffers(),
        swapChain->getExtent(),
        pipeline->getPipeline(),
        *uniformBuffer,
        renderList
    );

    ubos = (DynamicUBO*)alignedAlloc(uniformBuffer->layouts[0].bufferSize, uniformBuffer->layouts[0].dynamicAlignment);

    for (int i = 0; i < renderList.size(); i++)
    {
        renderList[i]->ubo = getUbo(i);
    }

    for (int i = 0; i < renderList.size(); i++)
    {
        renderList[i]->ubo->model = glm::mat4x4(1);
    }

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
        vao->destroy();
        delete vao;
    }

    destroyUniformBuffer();
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

    static float theta { 0.0f };

    theta += time.getDeltaTime() * 100.0f;

    glm::mat4x4& model_0 = renderList[0]->ubo->model;
    glm::mat4x4& model_1 = renderList[1]->ubo->model;
    glm::mat4x4& model_2 = renderList[2]->ubo->model;

    model_0 = glm::mat4x4(1);
    model_0 = glm::translate(model_0, glm::vec3(1, 0, -10));
    model_0 = glm::rotate(model_0, glm::radians(theta), glm::vec3(1, 0, 0));
    model_0 = glm::rotate(model_0, glm::radians(theta), glm::vec3(0, 1, 0));
    model_0 = glm::rotate(model_0, glm::radians(theta), glm::vec3(0, 0, 1));

    model_1 = glm::mat4x4(1);
    model_1 = glm::translate(model_1, glm::vec3(3, 0, -20));
    model_1 = glm::rotate(model_1, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model_1 = glm::rotate(model_1, glm::radians(theta * 1.0f), glm::vec3(0, 0, 1));
    model_1 = glm::scale(model_1, glm::vec3(0.01f));

    model_2 = glm::mat4x4(1);
    model_2 = glm::translate(model_2, glm::vec3(1, 0, -10));
    model_2 = glm::rotate(model_2, glm::radians(theta), glm::vec3(1, 0, 0));
    model_2 = glm::rotate(model_2, glm::radians(theta), glm::vec3(0, 1, 0));
    model_2 = glm::rotate(model_2, glm::radians(theta), glm::vec3(0, 0, 1));

    static float timer { 0.0f };

    timer += time.getDeltaTime();

    if (timer >= 1.0f)
    {
        window.setTitle(GPU::about.name + ", fps: " + std::to_string(time.getFps()));

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

    uniformBuffer->updateUniformBuffer(swapChain->getImageIndex(), 0, uniformBuffer->layouts[0].dynamicAlignment * uniformBuffer->layouts[0].instances, ubos);
    
    swapChain->acquireImage();
    swapChain->syncImagesInFlight();
    swapChain->submit(commandBuffer->getCommandBuffers());
    swapChain->present();

    time.endFrame();
}

void AstrumVK::awaitDeviceIdle()
{
    vkDeviceWaitIdle(GPU::getDevice());
}

void AstrumVK::onViewportResize(unsigned int newWidth, unsigned int newHeight)
{
    awaitDeviceIdle();

    GPU::recheckDeviceCapabilities();

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