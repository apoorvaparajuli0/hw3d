#include "Window.h"

/*
* entry point for the program (using "CALLBACK" or "WINAPI" in signature are interchangeable)
* 
* @param hInstance      -
* @param hPrevInstance  -
* @param lpCmdLine      -
* @param nCmdShow       -
*/
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow)
{
	try
	{
		//can have multiple instances for multiple windows if needed
		Window wnd(800, 300, "Test");

		MSG msg;
		BOOL gResult;

		//GetMessage returns -1 if failed, or whatever return code is provided in a message if succesfull
		while (gResult = GetMessage(&msg, nullptr, 0, 0) > 0) {

			//generates WM_CHAR messages out of my message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (wnd.kbd.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr, 
					"Something Is Happening", 
					"Alt Has Been Pressed",
					 MB_OK | MB_ICONEXCLAMATION);
			}
		}

		if (gResult == -1) {
			return -1;
		}

		return msg.wParam;

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