#include "MeshAsset.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

MeshAsset* loadModelAsset(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene)
    {
        DebugLogOut(importer.GetErrorString());
    }

    MeshAsset* result = new MeshAsset();

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