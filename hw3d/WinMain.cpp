#include "Window.h"
#include "App.h"

/*
* entry point for the program (using "CALLBACK" or "WINAPI" in signature are interchangeable)
* 
* @param hInstance      -handle to an instance of our window I guess?
* @param hPrevInstance  -handle to the previous instance of our window
*                        I would imaging
* @param lpCmdLine      -the command line that will be used for the app
*                        passed as an LPSTR
* @param nCmdShow       -determines how window is shown, maybe look at
*                        ShowWindow() function docs on MSDN for more info
*/
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow) 
{
	try
	{
		return App().Go();

	}
	//different cases for a MyException, std::exception or other error
	catch (const MyException& e)
	{
		MessageBox(nullptr, e.what(),
			e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{ 
		MessageBox(nullptr, e.what(), 
			"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details Available",
			"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
}