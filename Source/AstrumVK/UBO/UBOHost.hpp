#ifndef __AstrumVK_UBO_UBOHost_hpp__
#define __AstrumVK_UBO_UBOHost_hpp__

#pragma once

#include "UniformBufferLayout.hpp"

void* alignedAlloc(size_t size, size_t alignment);

template <typename T, bool dynamic = false>
class UBOHost
{
private:
    T* data;

    size_t dynamicAlignment;
    size_t bufferSize;
    size_t size;

    uint32_t instances;
    uint32_t memoryChunk;

    std::vector<VkBuffer>* uniformBuffers;
    std::vector<VkDeviceMemory>* uniformBuffersMemory;

public:
    inline void allocate(UniformLayout& layout)
    {
        dynamicAlignment = layout.dynamicAlignment;
        size = layout.bufferSize;
        instances = layout.instances;
        memoryChunk = dynamicAlignment * instances;
        uniformBuffers = &layout.uniformBuffers;
        uniformBuffersMemory = &layout.uniformBuffersMemory;

        if (dynamic)
        {
            data = static_cast<T*>(alignedAlloc(size, dynamicAlignment));
        }
        else
        {
            data = new T();
        }
    }

    T* operator[](int i)
    {
        return (T*)((uint64_t)data + (i * dynamicAlignment));
    }  

    inline void update(int i)
    {
        if (dynamic)
        {
            void* data;
            vkMapMemory(GPU::getDevice(), (*uniformBuffersMemory)[i], 0, memoryChunk, 0, &data);
                memcpy(data, this->data, memoryChunk);
            vkUnmapMemory(GPU::getDevice(), (*uniformBuffersMemory)[i]);
        }
        else
        {
            void* data;
            vkMapMemory(GPU::getDevice(), (*uniformBuffersMemory)[i], 0, sizeof(T), 0, &data);
                memcpy(data, this->data, sizeof(T));
            vkUnmapMemory(GPU::getDevice(), (*uniformBuffersMemory)[i]);
        }
    }
};

#endif