#pragma once
#include "Windows.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MyException.h"
#include "Graphics.h"
#include <optional>
#include <memory>


class Window
{
public:
	class Exception : public MyException
	{
	public :
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		static std::string TranslateErrorCode( HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private :
		HRESULT hr;
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
	void SetTitle(std::string title) const noexcept;
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT HandleMsg (HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};

#define WND_EXCEPT(hr) Window::Exception( __LINE__,__FILE__, hr)
#define WND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())