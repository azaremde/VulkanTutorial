#ifndef __Pch_hpp__
#define __Pch_hpp__

#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <set>

#define FORCE_DEBUG_MESSAGES

#ifdef NDEBUG
    #define RELEASE_CONFIG
#else
    #define DEBUG_CONFIG
#endif

#define LogOut(x) std::cout << x << '\n'
#define ErrOut(x) std::cerr << x << '\n'

#if defined(DEBUG_CONFIG) || defined(FORCE_DEBUG_MESSAGES)
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