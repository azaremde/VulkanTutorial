#include "ImageAsset.hpp"

ImageAsset* loadImageAsset(const std::string& filename)
{
    ImageAsset* result = new ImageAsset();

    result->pixels = stbi_load(filename.c_str(), &result->texWidth, &result->texHeight, &result->texChannels, STBI_rgb_alpha);

    return result;
}