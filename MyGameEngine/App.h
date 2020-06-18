#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"

class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};

