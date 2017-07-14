#include "App.h"



App::App()
{
	pWin32Window = new Win32Window(GetModuleHandle(NULL) , true, 1024, 768, "WindowTest");
	pInputSystem = new InputSystem();
	pRenderer = new HelloFullscreen(1024,768);
}


App::~App()
{
}


void App::Initialize()
{
	pInputSystem->Initialize();
	pWin32Window->Initialize();
	pRenderer->Initialize();
}

void App::Update(float dt)
{
	do
	{
		pWin32Window->Update(dt);
		pRenderer->Update(dt);
		pRenderer->Render();
	} while (!pWin32Window->quit_);
}

void App::Shutdown()
{
	pWin32Window->Shutdown();
	pInputSystem->Shutdown();
	pRenderer->Shutdown();

	delete pWin32Window;
	delete pInputSystem;
	delete pRenderer;
}


int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPreviousInstance, LPSTR, int show)
{

	App * pApp = new App();

	pApp->Initialize();

	pApp->Update(0.032f);

	pApp->Shutdown();

}