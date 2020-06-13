#pragma once
#include <d3d11.h>
#include "MyException.h"
#include "DxgiInfoManager.h"

class Graphics
{
public:
	class Exception : public MyException
	{
	public:
		Exception(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	class DeviceRemovedException : public Exception
	{
		using Exception::Exception;
	public:
		const char* GetType() const noexcept override;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain * pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};
