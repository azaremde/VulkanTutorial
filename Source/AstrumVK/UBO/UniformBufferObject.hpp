#ifndef __AstrumVK_UBO_UniformBufferObject_hpp__
#define __AstrumVK_UBO_UniformBufferObject_hpp__

#pragma once

#include "Pch.hpp"

struct UniformBufferObject
{
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;
};

#endif