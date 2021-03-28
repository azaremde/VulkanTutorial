#include "Window.hpp"

Window::Window(unsigned int _width, unsigned int _height, const std::string& _title)
{
    width = _width;
    height = _height;
    title = _title;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pGlfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    DebugLogOut("Window created.");
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(pGlfwWindow);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

Window::~Window()
{
    glfwDestroyWindow(pGlfwWindow);
    glfwTerminate();

    DebugLogOut("Window destroyed.");
}

unsigned int Window::getWidth() const
{
    return width;
}

unsigned int Window::getHeight() const
{
    return height;
}

std::string Window::getTitle() const
{
    return title;
}

GLFWwindow* Window::getGlfwWindow()
{
    return pGlfwWindow;
}