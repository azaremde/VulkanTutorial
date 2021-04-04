#include "AstrumVK.hpp"

#include "Debug.hpp"

#include "Assets/ImageAsset.hpp"
#include "Assets/MeshAsset.hpp"

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
    Surface::create(&instance, &window);
}

void AstrumVK::destroySurface()
{
    Surface::destroy();
}

void AstrumVK::createGPU()
{
    GPU::create(&instance);
}

void AstrumVK::destroyGPU()
{
    GPU::destroy();
}

void AstrumVK::createSwapChain()
{
    swapChain = new SwapChain(window, true);
}

void AstrumVK::destroySwapChain()
{
    delete swapChain;
}

void AstrumVK::createPipeline()
{
    uint32_t amountOfModels { 3 };

    UniformLayout dynamicLayout{};
    UniformLayout staticLayout{};
    UniformLayout imageLayout{};

    dynamicLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    dynamicLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    dynamicLayout.binding = 0;
    dynamicLayout.instances = amountOfModels; // Should be equal to the amount of models in the scene.

    staticLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    staticLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    staticLayout.binding = 1;
    staticLayout.size = sizeof(StaticUBO);

    imageLayout.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    imageLayout.binding = 2;
    imageLayout.size = 1;

    defaultShader = new Shader("Assets/Shaders/DefaultShader.vert.spv", "Assets/Shaders/DefaultShader.frag.spv");

    pipeline = new Pipeline(*swapChain, *defaultShader, { dynamicLayout, staticLayout, imageLayout });
}

void AstrumVK::destroyPipeline()
{
    delete pipeline;

    delete defaultShader;
}

void AstrumVK::createModels(const std::vector<ModelDescriptor>& models)
{
    for (int i = 0; i < models.size(); i++)
    {
        MeshAsset* model = loadModelAsset(models[i].modelFilename);
        ImageAsset* image = loadImageAsset(models[i].textureFilename);

        Entity* entity = new Entity();
        commandBuffer->createVertexBuffer(entity, model->vertices);
        commandBuffer->createIndexBuffer(entity, model->indices);
        entity->albedoColor = new Texture2D(*commandBuffer, image);

        pipeline->getUniformLayouts()[2].imageViews.emplace_back(entity->albedoColor->getImageView());
        pipeline->getUniformLayouts()[2].samplers.emplace_back(entity->albedoColor->getSampler());

        renderList.emplace_back(entity);

        delete model;
        delete image;
    }

    uniformBuffer = new UniformBuffer(*swapChain, *pipeline, renderList);
    dynamicUbos.allocate(pipeline->getUniformLayouts()[0]);
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
    commandBuffer = new CommandBuffer(*swapChain);
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

    createModels({
        { "Assets/Models/cube.obj", "Assets/Images/texture.jpg" },
        { "Assets/Models/base.fbx", "Assets/Images/diffuse.png" },
        { "Assets/Models/base.fbx", "Assets/Images/diffuse.png" }
    });

    commandBuffer->render(*pipeline, swapChain->getFramebuffers(), swapChain->getExtent(), renderList);

    // For the simplicity sake we're giving each entity pointer to its ubo.
    for (int i = 0; i < renderList.size(); i++)
    {
        renderList[i]->ubo = dynamicUbos[i];
        renderList[i]->ubo->model = Mat4(1);
    }

    staticUbos.allocate(pipeline->getUniformLayouts()[1]);

    staticUbos[0]->proj = glm::perspective(
        glm::radians(70.0f),
        window.getRatio(),
        0.1f, 
        1000.0f
    );

    staticUbos[0]->view = Mat4(1);

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++)
    {
        staticUbos.update(i);
    }
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

    Mat4& model_0 = renderList[0]->ubo->model;
    Mat4& model_1 = renderList[1]->ubo->model;
    Mat4& model_2 = renderList[2]->ubo->model;

    model_0 = Mat4(1);
    model_0 = glm::translate(model_0, Vec3(1, 0, -10));
    model_0 = glm::rotate(model_0, glm::radians(theta), Vec3(0, 1, 0));

    model_1 = Mat4(1);
    model_1 = glm::translate(model_1, Vec3(0, 0, 0));
    model_1 = glm::rotate(model_1, glm::radians(-90.0f), Vec3(1, 0, 0));
    model_1 = glm::rotate(model_1, glm::radians(theta * 1.0f), Vec3(0, 0, 1));
    model_1 = glm::scale(model_1, Vec3(0.01f));

    model_2 = Mat4(1);
    model_2 = glm::translate(model_2, Vec3(1, 0, -10));
    model_2 = glm::rotate(model_2, glm::radians(theta), Vec3(0, 1, 0));

    staticUbos[0]->view = glm::lookAt(
        Vec3(25 * glm::sin(glm::radians(theta / 10.0f)), -10, 25 * glm::cos(glm::radians(theta / 10.0f))),
        Vec3(0),
        Vec3(0, 1, 0)
    );
    
    staticUbos.update(swapChain->getImageIndex());

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

        commandBuffer->render(*pipeline, swapChain->getFramebuffers(), swapChain->getExtent(), renderList);
    }
    
    dynamicUbos.update(swapChain->getImageIndex());
    
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

    commandBuffer->render(*pipeline, swapChain->getFramebuffers(), swapChain->getExtent(), renderList);

    staticUbos[0]->proj = glm::perspective(
        glm::radians(70.0f), 
        window.getRatio(),
        0.1f, 
        1000.0f
    );

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++)
    {
        staticUbos.update(i);
    }
}