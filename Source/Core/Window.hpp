#ifndef __Core_Window_hpp__
#define __Core_Window_hpp__

#include "Pch.hpp"

struct IOnViewportResize
{
	virtual void OnViewportResize(int newWidth, int newHeight) = 0;
};

class Window
{
private:
	GLFWwindow*		window;

	unsigned int	width;
	unsigned int	height;
	std::string		title;

	static void OnWindowResizeCallback_(GLFWwindow* window, int newWidth, int newHeight);
	void OnWindowResizeCallback(int newWidth, int newHeight);

	std::vector<IOnViewportResize*> onWindowResizeSubscribers;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

public:
	Window(const unsigned int _width, const unsigned int _height, const std::string& _title);
	~Window();

	const bool ShouldClose() const;
	void PollEvents() const;

	GLFWwindow* GetGlfwWindow();

	void AddOnWindowResizeSubscriber(IOnViewportResize* subscriber);
	void QueryFramebufferSize(int* width, int* height);
};

#endif