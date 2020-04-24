#include "Window.h"

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
		throw Exception("Windows Rect failed");
	}

	hWnd = CreateWindowExA(
		0, Window::WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, Window::WindowClass::GetInstance(), this);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{}

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

