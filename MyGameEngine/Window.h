#pragma once
#include "Windows.h"
#include "Keyboard.h"
#include <exception>
#include <optional>

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
private:
	//singleton
	class WindowClass
	{
	public:
		static const char * GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "MyWindowClass";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	static std::optional<int> ProcessMessages() noexcept;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT HandleMsg (HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
public:
	Keyboard kbd;
private:
	int width;
	int height;
	HWND hWnd;
};

