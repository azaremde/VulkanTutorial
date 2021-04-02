#ifndef __Models_Model_hpp__
#define __Models_Model_hpp__

#pragma once

#include "Pch.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using Vec3 = glm::vec3;
using Vec2 = glm::vec2;

struct Vert
{
    Vec3 position;
    Vec2 textureCoords;
    Vec3 normal;
    
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vert);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vert, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vert, textureCoords);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vert, normal);

        return attributeDescriptions;
    }
};

struct Model
{
    std::vector<Vert> vertices;
    std::vector<uint32_t> indices;
};

inline static Model* loadModel(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene)
    {
        DebugLogOut(importer.GetErrorString());
    }

    Model* result = new Model();

    aiMesh* mesh = scene->mMeshes[0];

    result->vertices.resize(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        result->vertices[i].position = Vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->mNormals != nullptr)
        {
            result->vertices[i].normal = Vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        if(mesh->HasTextureCoords(0))
        {
            result->vertices[i].textureCoords = Vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            result->vertices[i].textureCoords = Vec2();
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        auto face = &mesh->mFaces[i];
        result->indices.emplace_back(face->mIndices[0]);
        result->indices.emplace_back(face->mIndices[1]);
        result->indices.emplace_back(face->mIndices[2]);
    }

    importer.FreeScene();

    return result;
}

#endif