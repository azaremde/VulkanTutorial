#ifndef __AstrumVK_Pipeline_InputAssemblyLayout_Vertex_hpp__
#define __AstrumVK_Pipeline_InputAssemblyLayout_Vertex_hpp__

#pragma once

#include "Pch.hpp"

#include "Math/Math.hpp"

struct Vertex
{
    Vec3 position;
    Vec2 textureCoords;
    Vec3 normal;
    
    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

#endif