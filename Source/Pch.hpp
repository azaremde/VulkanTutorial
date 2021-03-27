#ifndef __Pch_h__
#define __Pch_h__

#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>

#include <iostream>
#include <fstream>

#include <cstdlib>
#include <cstdint>

#include <stdexcept>
#include <cassert>

#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define LogOut(x) std::cout << x << '\n'
#define ErrOut(x) std::cerr << x << '\n'

#ifndef NDEBUG
#	define DebugLogOut(x) std::cout << x << '\n'
#else
#	define DebugLogOut(x) ((void)0)
#endif

#define VulkanCheck(what, err)\
	if (what != VK_SUCCESS)\
	{\
		throw std::runtime_error(err);\
	}

#define RemoveCopyConstructor(Class)\
	Class(const Class&) = delete;\
	Class& operator=(const Class&) = delete

#endif