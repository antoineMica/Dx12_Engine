#pragma once
#include "ISystem.h"
#include "InputSystem.h"
#include "Win32Window.h"
#include "DX12HelloTexture.h"

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
	DX12HelloTexture * pRenderer;
};

