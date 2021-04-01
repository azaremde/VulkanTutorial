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

bool GPU::isDeviceSuitable(const VkPhysicalDevice& physDevice) {
    queues.familyIndices = QueueFamilyIndices::of(physDevice, surface.getSurface());

    bool extensionsSupported = checkDeviceExtensionsSupport(physDevice);

    if (extensionsSupported)
    {
        swapChainSupport = SwapChainSupportDetails::of(physDevice, surface.getSurface());
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physDevice, &props);

    limits.minUniformBufferOffsetAlignment = props.limits.minUniformBufferOffsetAlignment;

    return queues.familyIndices.isComplete() && swapChainSupport.isSupported() && extensionsSupported /* redundant */;
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