#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		return App{}.Go();
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr,e.what(), "Standard exception", MB_OK | MB_ICONEXCLAMATION);
	}
}