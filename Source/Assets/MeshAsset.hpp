#ifndef __Assets_MeshAsset_hpp__
#define __Assets_MeshAsset_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/Pipeline/InputAssemblyLayouts/Vertex.hpp"

struct MeshAsset
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

MeshAsset* loadModelAsset(const std::string& path);

#endif