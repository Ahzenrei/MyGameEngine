#pragma once
#include "Windows.h"
#include <exception>

class Window
{
public:
	class Exception : public std::exception
	{
	public :
		Exception(const char * str)
			:
			buffer(str)
		{}
		const char * what() const throw () {
			return buffer;
		}
	private:
		const char * buffer;
	};

	Window(int width, int height, const char* name);
	~Window();
private:
	class WindowClass
	{
	public:
		static const char * GetName();
		static HINSTANCE GetInstance();
	private:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass &) = delete;
		WindowClass operator=(const WindowClass) = delete;
		static const HINSTANCE hInstance;
		static WNDCLASSEX wc;
	};
private:
	int width;
	int height;
	HWND hWnd;
};

