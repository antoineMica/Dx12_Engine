#pragma once
#include "ISystem.h"
#include "InputSystem.h"
#include "Win32Window.h"
#include "HelloFullscreen.h"

class App :	public ISystem
{
public:
	App();
	~App();

	void Initialize();
	void Update(float dt);
	void Shutdown();


	Win32Window * pWin32Window;
	InputSystem * pInputSystem;
	HelloFullscreen * pRenderer;
};

