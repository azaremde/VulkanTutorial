#ifndef __Window_hpp__
#define __Window_hpp__

#pragma once

#include "Pch.hpp"

class IOnViewportResize
{
public:
    virtual void onViewportResize(unsigned int newWidth, unsigned int newHeight) = 0;
};

class Window
{
private:
    unsigned int    width;
    unsigned int    height;
    std::string     title;

    GLFWwindow*     pGlfwWindow;

    std::vector<IOnViewportResize*> onViewportResizeSubscribers;
    void onViewportResize(int newWidth, int newHeight);

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

public:
    Window(unsigned int _width, unsigned int _height, const std::string& _title);
    ~Window();

    void addOnViewportResizeSubscriber(IOnViewportResize* subscriber);

    bool shouldClose() const;
    
    void pollEvents() const;

    unsigned int    getWidth() const;
    unsigned int    getHeight() const;
    std::string     getTitle() const;
    GLFWwindow*     getGlfwWindow();
};

#endif