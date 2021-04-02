#include "GPU.hpp"

#include "AstrumVK/Debug.hpp"

void GPU::pickPhysicalDevice()
{    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) 
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) 
    {
        if (isDeviceSuitable(device)) 
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) 
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    DebugLogOut("Suitable physical device has been found.");
}

void GPU::createLogicalDevice()
{
    queues.familyIndices = QueueFamilyIndices::of(physicalDevice, surface.getSurface());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { queues.familyIndices.graphics.value(), queues.familyIndices.present.value() };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queues.familyIndices.graphics.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (Debug::validationLayersEnabled)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(Debug::validationLayers.size());
        createInfo.ppEnabledLayerNames = Debug::validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VK_CHECK(
        vkCreateDevice(physicalDevice, &createInfo, nullptr, &device),
        "Failed to create logical device."
    );

    DebugLogOut("Logical device successfully created.");
}

void GPU::destroyLogicalDevice()
{
    vkDestroyDevice(device, nullptr);
}

void GPU::retrieveQueues()
{
    vkGetDeviceQueue(device, queues.familyIndices.graphics.value(), 0, &queues.graphics);
    vkGetDeviceQueue(device, queues.familyIndices.present.value(), 0, &queues.present);
}

GPU::GPU(VkInstance& _instance, Surface& _surface) : instance { _instance }, surface { _surface }
{
    pickPhysicalDevice();
    createLogicalDevice();
    retrieveQueues();
}

GPU::~GPU()
{
    destroyLogicalDevice();

    DebugLogOut("Logical device has been destroyed.");
}

void GPU::recheckDeviceCapabilities()
{
    queues.familyIndices = QueueFamilyIndices::of(physicalDevice, surface.getSurface());

    bool extensionsSupported = checkDeviceExtensionsSupport(physicalDevice);

    if (extensionsSupported)
    {
        swapChainSupport = SwapChainSupportDetails::of(physicalDevice, surface.getSurface());
    }
}

uint32_t GPU::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void GPU::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(getDevice(), buffer, bufferMemory, 0);
}

void GPU::createImage(
    uint32_t width, 
    uint32_t height, 
    VkFormat format, 
    VkImageTiling tiling, 
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags properties, 
    VkImage& image, 
    VkDeviceMemory& imageMemory
)
{    
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(
        vkCreateImage(device, &imageInfo, nullptr, &image),
        "Failed to create image.");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    VK_CHECK(
        vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory), 
        "Failed to allocate image memory.");

    vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView GPU::createImageView(VkImage image, VkFormat format) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

bool GPU::isDeviceSuitable(const VkPhysicalDevice& physDevice) {
    queues.familyIndices = QueueFamilyIndices::of(physDevice, surface.getSurface());

    bool extensionsSupported = checkDeviceExtensionsSupport(physDevice);

    if (extensionsSupported)
    {
        swapChainSupport = SwapChainSupportDetails::of(physDevice, surface.getSurface());
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physDevice, &supportedFeatures);

    vkGetPhysicalDeviceProperties(physDevice, &props);
    limits.minUniformBufferOffsetAlignment = props.limits.minUniformBufferOffsetAlignment;

    return  queues.familyIndices.isComplete() && 
            swapChainSupport.isSupported() && 
            supportedFeatures.samplerAnisotropy && 
            extensionsSupported /* redundant */;
}

bool GPU::checkDeviceExtensionsSupport(const VkPhysicalDevice& device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

const VkQueue& GPU::getGraphicsQueue() const
{
    return queues.graphics;
}

const VkQueue& GPU::getPresentQueue() const
{
    return queues.present;
}

const VkPhysicalDevice& GPU::getPhysicalDevice() const
{
    return physicalDevice;
}

const VkDevice& GPU::getDevice() const
{
    return device;
}

const QueueFamilyIndices& GPU::getQueueFamilyIndices() const
{
    return queues.familyIndices;
}

const SwapChainSupportDetails& GPU::getSwapChainSupportDetails() const
{
    return swapChainSupport;
}