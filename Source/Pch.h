#ifndef __Pch_h__
#define __Pch_h__

#pragma once

#include <string>
#include <vector>
#include <map>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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