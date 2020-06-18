#include "Window.h"
#include <sstream>
#include "WindowsThrowMacros.h"
#include "imgui/imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle( nullptr ))
{
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

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	//Width and Height represent the client region size, not the whole windows size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
	//create window and register hwnd
	hWnd = CreateWindowExA(
		0, Window::WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, Window::WindowClass::GetInstance(), this);
	//error checking
	if (hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	
	//Init ImGui32
	ImGui_ImplWin32_Init(hWnd);

	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept 
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCT * const pCreate = reinterpret_cast<CREATESTRUCT *>(lparam); //the lparam of the WM_NCCREATE message is CREATESTRUCT that contain information about the window
		Window* const pWnd = reinterpret_cast<Window*>(pCreate->lpCreateParams); //lpCreateParams is the lparam that we pass at the window creation (which was this so it is the window)
		//We can store some data in the WinAPI userdata that we will use to acces HandleMessage
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//We change de window proc to HandleMsgThunk now that we have setup the user data
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		//now we pass it to the actual HandleMsg that we can acces with the window pointer
		return pWnd->HandleMsg(hWnd, msg, wparam, lparam);
	} 
	
	return DefWindowProc(hWnd, msg, wparam, lparam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wparam, lparam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd,msg, wparam, lparam))
	{
		return true;
	}
	switch (msg)
	{
		//we don't want the DefProc to handle this message because
		//we want our destructor to destroy the window 
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KILLFOCUS: //if we don't, when we loose the focus and a key is pressed, it will remain pressed
			kbd.ClearState();
		case WM_KEYDOWN:
			kbd.OnKeyPressed(wparam);
			break;
		case WM_KEYUP:
			kbd.OnKeyReleased(wparam);
			break;
		case WM_CHAR:
			kbd.OnChar(wparam);
			break;
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lparam);
			mouse.OnMouseMove(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lparam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lparam);
			mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lparam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lparam);
			mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL :
		{
			const POINTS pt = MAKEPOINTS(lparam);
			if (GET_WHEEL_DELTA_WPARAM(wparam) > 0)
			{
				mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wparam) < 0)
			{
				mouse.OnWheelDown(pt.x, pt.y);
			}
			break;
		}
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

void Window::SetTitle(std::string title) const noexcept
{
	SetWindowTextA(hWnd, title.c_str());
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw std::exception ("pGfx is null");
	}
	return *pGfx;
}

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	MyException(line, file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Windows exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		reinterpret_cast<LPTSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}