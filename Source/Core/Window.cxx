#include "Window.hpp"

Window::Window(const unsigned int _width, const unsigned int _height, const std::string& _title) : width{ _width }, height{ _height }, title{ _title }
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE,	GLFW_FALSE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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