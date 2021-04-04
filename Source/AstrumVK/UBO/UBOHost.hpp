#ifndef __AstrumVK_UBO_UBOHost_hpp__
#define __AstrumVK_UBO_UBOHost_hpp__

#pragma once

#include "UniformBufferLayout.hpp"

void* alignedAlloc(size_t size, size_t alignment);

template <typename T>
class UBOHost
{
private:
    T* data;

    size_t dynamicAlignment;
    size_t size;

public:
    UniformLayout layout;

    inline void allocate(size_t _size, size_t _dynamicAlignment)
    {
        dynamicAlignment = _dynamicAlignment;
        size = _size;

        data = static_cast<T*>(alignedAlloc(size, dynamicAlignment));
    }

    T* operator[](int i)
    {
        return (T*)((uint64_t)data + (i * dynamicAlignment));
    }  

    inline void update(int i)
    {        
        void* data;
        vkMapMemory(GPU::getDevice(), layout.uniformBuffersMemory[i], 0, layout.dynamicAlignment * layout.instances, 0, &data);
            memcpy(data, this->data, layout.dynamicAlignment * layout.instances);
        vkUnmapMemory(GPU::getDevice(), layout.uniformBuffersMemory[i]);
    }
};

#endif