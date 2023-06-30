#include "Window.h";

//no idea what this does
Window::WindowClass Window::WindowClass::wndClass;

/*
* our constructor for our window class, registers
* our window and sets up the config struct
*/
Window::WindowClass::WindowClass() noexcept
	:
	//used to identify our application when
	//make further winAPI calls?
	hInst(GetModuleHandle(nullptr))
{ 
	//our window class configuration struct
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);
}

/*
* destructor which unregisters our window
*/
Window::WindowClass::~WindowClass() 
{
	UnregisterClass(wndClassName, GetInstance());
}

/*
* returns the name of our window class, stored as a member
* variable
*/
const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

/*
* returns an hInstance to identify our application instance
*/
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

/*
* constructor for the literal window, initializes our rectangle
* which represents the initial window
* 
* @param width  -used to determine width of screen
* @param height -used to determine height of screen
* @param name   -the name of the window that'll appear in the
*				 non-client region
*/
Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	//determines window size for the client region
	//used in CreateWindow to further calculate screen size
	// 
	//(Not sure why we're not just directly entering width and height into CreateWindow
	//maybe related to device unit conversion)
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top; 

	if ((AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) == 0)
	{
		throw MYWND_LAST_EXCEPT(); 
	}

	//config settings allowing us to minimize and add captions to our rectangle
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left,
		wr.bottom - wr.top, nullptr, nullptr, WindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr) throw MYWND_LAST_EXCEPT();

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

/*
* destructor for our window
*/
Window::~Window()
{
	DestroyWindow(hWnd);
}

/*
* helper for setting title of window, used when
* making sure keyboard/mouse input is triggering
* events
* 
* @param title -title of window
*/
void Window::SetTitle(const std::string title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw MYWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	//we're using peekmessage instead of getmessage because
	//it instantly returns when a message isn't received
	//unlike getmessage which will pause everything until a 
	//message is received

	//this loop just loops through the message queue and 
	//dispatches them
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		//check if we have a quit message manually
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//if we're not quitting the app then return empty option
	return {};
}

/*
* handles message handling setup the first time we get a message
* to our window
* 
* @param hWnd   -pointer to structure that contains window data I guess? 
*                some kind of handle to our window
* @param msg    -message we're sending
* @param wParam -not important here
* @param lParam -described below
*/
LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	//use the "this" passed in via CreateWindow() inside the
	//window constructor
	if (msg == WM_NCCREATE)
	{
		//lParam here is meant to be used as a pointer to the "create" structure so
		//we're reinterpreting it to that here, and lpCreateParams which is a 
		//member of the pCreate struct is actually meant to be used as a ptr to
		//a Window
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

			//so there's "user data" we can store using this function on the API
			//side, so essentially we're storing a pointer to our window class, 
			//in the API and by doing this we're creating a link between our window
			//class and the window itself
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

			//we're setting the message handling procedure to be the normal
			//non-setup procedure by redirecting GWLP_WNDPROC which directs
			//the window procedure to call our handleMsgThunk()
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, 
				reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

			//forwarding message to handleMsg
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
}

/*
* basically just a middleman function to get a pointer to our
* actual window which we stored in the WinAPI as user data, and
* then to invoke the actual message handler
* 
* @param hWnd   -handle to our window
* @param msg    -our message
* @param wParam -not imporant here
* @param lParam -not important here
*/
LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	//we get our ptr to our window class from the API sides user data
	//(remember we stored the ptr there in HandleMsgSetup)
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	//here we forward our message to the actual handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

/*
* our actual message handler, we switch between message types and
* behave accordingly
* 
* @param hWnd   -handle to our window
* @param msg    -message
* @param wParam -is the keyboard input code
* @param lParam -contains bitflags for msgInput handling
*/
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam) 
{

	//and now we finally get to actually handling individual messages
	//by switching through different msg formats and acting accordingly
	switch (msg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;

		case WM_KEYDOWN:
		//we need to take into account syskeys
		//because keys like alt or f10 are system
		//keys
		case WM_SYSKEYDOWN: 
			//check bit 30 of lParam which will indicate to us
			//whether the WM_KEYDOWN message is for the same key
			//as it was in the last message
			if (!(lParam & 0x40000000 || kbd.AutorepeatIsEnabled()))
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
		case WM_MOUSEMOVE:
		{
			//lParam represents coordinates when
			//passed into MAKEPOINTS which unpacks
			//the bits to get the actual values I guess
			const POINTS pt = MAKEPOINTS(lParam);

			//check if we're in the client region of the window
			if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
			{
				//logging mouse movement
				mouse.OnMouseMove(pt.x, pt.y);

				if (!mouse.IsInWindow())
				{
					//set capture will allow us to keep track of mouse
					//position within our window
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
			}
			//we're not in client region of window
			else
			{
				//this checks to see if either left or right mouse buttons
				//are being pressed since wParam contains mouse info and the
				//mk's let us know if either button is being pressed
				//(this specifically checks to see if we're dragging the window)
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					//we've truly exited the client region and we're not
					//dragging, so we release the mouse and set it as 
					//having left
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}

			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);

			//wParam is used as a value to calculate
			//whether the mouse is moving up or down
			//when passed to GET_WHEEL_DELTA_WPARAM
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(pt.x, pt.y, delta);
			break;
		}
	
	}

	//we don't need to call destroy window manually,
	//that's handled automatically by our destructor when
	//our window goes out of scope
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*
* initializes our window exception class
* 
* @param line -the line where error is thrown
* @param file -file error is thrown
* @param hr   -an hresult, pretty much some kind
*              of built in windows error code/message
*/
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	MyException(line, file),
	hr(hr)
{}

/*
* overriden function that just returns our exception details and fills
* up the whatBuffer with the information
*/
const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[ERROR CODE]: " << GetErrorCode() << std::endl
		<< "[DESCRIPTION]: " << GetErrorString() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

/*
* identifies exception as a window class exception
*/
const char* Window::Exception::GetType() const noexcept
{
	return "My Window Exception";
}

/*
* formats our error message and returns the error message as a string
* 
* @param hr -takes the hresult as a parameter
*/
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;

	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);

	if (nMsgLen == 0)
	{
		return "Unexpected/Unidentified Error Code";
	}

	std::string errorString = pMsgBuf;

	//not sure why we're doing this
	LocalFree(pMsgBuf);

	return errorString;
}

/*
* returns the hresult which is the error code
*/
HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

/*
* returns the error string
*/
std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}