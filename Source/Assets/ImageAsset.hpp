#ifndef __Assets_ImageAsset_hpp__
#define __Assets_ImageAsset_hpp__

#pragma once

#include "Pch.hpp"

#include <stb_image/stb_image.h>

struct ImageAsset
{
    stbi_uc* pixels;
    int texWidth, texHeight, texChannels;

    ImageAsset() {}

    ~ImageAsset()
    {
        stbi_image_free(pixels);
    }

    ImageAsset(ImageAsset&) = delete;
    ImageAsset& operator=(ImageAsset&) = delete;
};

inline static ImageAsset* loadImageAsset(const std::string& filename)
{
    ImageAsset* result = new ImageAsset();

    result->pixels = stbi_load(filename.c_str(), &result->texWidth, &result->texHeight, &result->texChannels, STBI_rgb_alpha);

    return result;
}

#endif