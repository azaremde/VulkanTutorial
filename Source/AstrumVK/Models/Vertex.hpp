#ifndef __AstrumVK_Models_Vertex_hpp__
#define __AstrumVK_Models_Vertex_hpp__

#pragma once

#include "Pch.hpp"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

#endif