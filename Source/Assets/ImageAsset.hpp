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

ImageAsset* loadImageAsset(const std::string& filename);

#endif