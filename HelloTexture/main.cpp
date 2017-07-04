#include "ThreadPool.h"
#include "App.h"
#include <iostream>

void RunApp(HINSTANCE instance)
{
	App * pApp = new App();

	pApp->Initialize();

	pApp->Update(0.032f);

	pApp->Shutdown();
}



int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPreviousInstance, LPSTR, int show)
{
	RunApp(instance);
	return 0;
}