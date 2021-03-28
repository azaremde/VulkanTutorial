#ifndef __Pch_hpp__
#define __Pch_hpp__

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <vector>
#include <iostream>

#ifdef NDEBUG
    #define RELEASE_CONFIG
#else
    #define DEBUG_CONFIG
#endif

#define LogOut(x) std::cout << x << '\n'
#define ErrOut(x) std::cerr << x << '\n'

#ifdef DEBUG_CONFIG
    #define DebugLogOut(x) std::cout << x << '\n'
#else
    #define DebugLogOut(x) ((void*)0)
#endif

#define VK_CHECK(what, err)\
    if (what != VK_SUCCESS)\
    {\
        throw std::runtime_error(err);\
    }

#endif