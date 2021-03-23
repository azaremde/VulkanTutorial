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

#define LogOut(x) std::cout << x << '\n'
#define ErrOut(x) std::cerr << x << '\n'

#define VulkanCheck(what, err)\
	if (what != VK_SUCCESS)\
	{\
		throw std::runtime_error(err);\
	}

#endif