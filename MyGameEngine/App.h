#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"

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
	float speed_factor = 1.0f;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight light;
	std::vector<std::unique_ptr<class Drawable>> drawables;

	static constexpr size_t nDrawables = 180;
};

