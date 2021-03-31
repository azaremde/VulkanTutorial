#ifndef __AstrumVK_Time_hpp__
#define __AstrumVK_Time_hpp__

#pragma once

#include "Pch.hpp"

class Time
{
private:
    float        lastTime  { 0.0f };
    float        deltaTime { 0.0f };
    unsigned int fps       { 0 };

public:
    void beginFrame();
    void endFrame();

    float getDeltaTime() const;
    unsigned int getFps() const;
};

#endif