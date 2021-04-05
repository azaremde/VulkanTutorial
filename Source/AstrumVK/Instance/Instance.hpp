#ifndef __AstrumVK_Instance_Instance_hpp__
#define __AstrumVK_Instance_Instance_hpp__

#include "Pch.hpp"

#include "Core/Window.hpp"

class Instance
{
private:
    inline static VkInstance instance;

public:
    static void create(Window& window);
    static void destroy();

    static const VkInstance& getInstance();
    
};

#endif