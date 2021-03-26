#include "Window.hpp"

Window::Window(const unsigned int _width, const unsigned int _height, const std::string& _title) : width{ _width }, height{ _height }, title{ _title }
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE,	GLFW_FALSE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, OnWindowResizeCallback_);
}

void Window::OnWindowResizeCallback_(GLFWwindow* window, int newWidth, int newHeight)
{
	Window& win = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	win.OnWindowResizeCallback(newWidth, newHeight);
}

void Window::OnWindowResizeCallback(int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;

	for (const auto& sub : onWindowResizeSubscribers)
	{
		sub->OnViewportResize(newWidth, newHeight);
	}
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

const bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(window);
}

void Window::PollEvents() const
{
	glfwPollEvents();
}

GLFWwindow* Window::GetGlfwWindow()
{
	return window;
}

void Window::QueryFramebufferSize(int* width, int* height)
{
	glfwGetFramebufferSize(window, width, height);
}

void Window::AddOnWindowResizeSubscriber(IOnViewportResize* subscriber)
{
	onWindowResizeSubscribers.emplace_back(subscriber);
}