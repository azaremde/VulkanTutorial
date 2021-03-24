#ifndef __Core_Window_hpp__
#define __Core_Window_hpp__

#include "Pch.hpp"

class Window
{
private:
	GLFWwindow*		window;

	unsigned int	width;
	unsigned int	height;
	std::string		title;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

public:
	Window(const unsigned int _width, const unsigned int _height, const std::string& _title);
	~Window();

	const bool ShouldClose() const;
	void PollEvents() const;

	GLFWwindow* GetGlfwWindow();

	void QueryFramebufferSize(int* width, int* height);
};

#endif