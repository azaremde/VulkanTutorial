#include "Core/App.hpp"

#ifdef WIN_MAIN_ENTRY
	#include <Windows.h>
#endif

#ifdef WIN_MAIN_ENTRY
	int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd)
#else
	int main()
#endif
{
	App app;
	
	try 
	{
		app.run();
	}
	catch (const std::exception& ex)
	{
		ErrOut(ex.what());

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}