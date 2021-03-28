#include "Core/App.hpp"

int main()
{
	App app;

	try 
	{
		app.run();
	}
	catch (const std::exception& ex)
	{
		ErrOut(ex.what());
	}
}