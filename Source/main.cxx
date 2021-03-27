#include "Core/App.hpp"

int main()
{
	auto app = App();

	try 
	{
		app.Run();
	}
	catch (std::exception e)
	{
		ErrOut(e.what());

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}