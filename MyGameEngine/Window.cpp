#include "Window.h"

Window::WindowClass::WindowClass()
{
	wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;

}

Window::WindowClass::~WindowClass()
{}

HINSTANCE Window::WindowClass::GetInstance()
{
	return hInstance;
}

const char * Window::WindowClass::GetName()
{
	return wc.lpszClassName;
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
		"MyWindow", name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass.GetInstance(), nullptr);
}

