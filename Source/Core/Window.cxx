#include "Window.hpp"

Window::Window(unsigned int _width, unsigned int _height, const std::string& _title)
{
    width = _width;
    height = _height;
    title = _title;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    pGlfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(pGlfwWindow, this);

    glfwSetWindowSizeCallback(pGlfwWindow, [](GLFWwindow* pGlfwWindow, int newWidth, int newHeight){
        Window& self = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(pGlfwWindow));
        self.onViewportResize(newWidth, newHeight);
    });

    DebugLogOut("Window created.");
}

void Window::onViewportResize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    for (const auto& sub : onViewportResizeSubscribers)
    {
        sub->onViewportResize(newWidth, newHeight);
    }
}

void Window::addOnViewportResizeSubscriber(IOnViewportResize* subscriber)
{
    onViewportResizeSubscribers.emplace_back(subscriber);
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

void Window::setTitle(const std::string& title)
{
    glfwSetWindowTitle(pGlfwWindow, title.c_str());
}

unsigned int Window::getWidth() const
{
    return width;
}

unsigned int Window::getHeight() const
{
    return height;
}

float Window::getRatio() const
{
    return static_cast<float>(width) / static_cast<float>(height);
}

std::string Window::getTitle() const
{
    return title;
}

GLFWwindow* Window::getGlfwWindow()
{
    return pGlfwWindow;
}