#include "Time.hpp"

void Time::beginFrame()
{
    lastTime = static_cast<float>(glfwGetTime());

    fps = static_cast<unsigned int>(1.0f / deltaTime);
}

void Time::endFrame()
{
    deltaTime = static_cast<float>(glfwGetTime()) - lastTime;
}

float Time::getDeltaTime() const
{
    return deltaTime;
}

unsigned int Time::getFps() const
{
    return fps;
}