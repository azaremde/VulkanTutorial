#ifndef __Pch_h__
#define __Pch_h__

#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>

#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

template <typename T>
using Avec		= std::vector<T>;

template <typename T1, typename T2>
using Amap		= std::map<T1, T2>;

using Astr		= std::string;

using Aint		= GLint;
using Auint		= GLuint;
using Afloat	= GLfloat;

#define AMlog(x) std::cout << x << '\n'

#endif