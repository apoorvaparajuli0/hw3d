#include "Window.h";

//no idea what this does
Window::WindowClass Window::WindowClass::wndClass;

/*
* 
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
*
*/
Window::WindowClass::~WindowClass() 
{
	UnregisterClass(wndClassName, GetInstance());
}

/*
*
*/
const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

/*
*
*/
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

/*
* @param width
* @param height
* @param name
*/
Window::Window(int width, int height, const char* name)
{
	//determines window size for the client region
	//(the actual inside of the window)
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top; 
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	throw MYWND_EXCEPT(ERROR_ARENA_TRASHED);

	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left,
		wr.bottom - wr.top, nullptr, nullptr, WindowClass::GetInstance(),
		this
	);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

/*
*
*/
Window::~Window()
{
	DestroyWindow(hWnd);
}

/*
* @param hWnd
* @param msg
* @param wParam
* @param lParam
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
* @param hWnd
* @param msg
* @param wParam
* @param lParam
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
* @param hWnd
* @param msg
* @param wParam
* @param lParam
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
	}

	//we don't need to call destroy window manually,
	//that's handled automatically by our destructor when
	//our window goes out of scope
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*
* 
* @param line
* @param file
* @param hr
*/
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	MyException(line, file),
	hr(hr)
{}

/*
*
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
*
*/
const char* Window::Exception::GetType() const noexcept
{
	return "My Window Exception";
}

/*
* @param hr
*/
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;

	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		pMsgBuf, 0, nullptr
	);

	if (nMsgLen == 0)
	{
		return "Unexpected/Unidentified Error Code";
	}

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);

	return errorString;
}

/*
*
*/
HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

/*
* 
*/
std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}