#pragma once
#include "Win.h"
#include "MyException.h"
#include <sstream>
#include "Keyboard.h"
#include "Mouse.h"
#include <optional>
#include <memory>
#include "Graphics.h"

class Window
{

public:
	class Exception : public MyException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;
	};
private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass(); //don't have to do this, but good practice I guess

		//I guess we're not allowing copy construction or initialziation for our Window
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator = (const WindowClass&) = delete;
		static constexpr const char* wndClassName = "D3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string);
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();

private:
	//for some reason, static functions can be registered as Windows procedures, like the upper
	//two callbacks below, but something like the HandleMsg cannot without extra work
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Keyboard kbd;
	Mouse mouse;

private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};

#define MYWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr);
#define MYWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError());
