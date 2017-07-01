#include "ThreadPool.h"
#include "App.h"
#include <iostream>

std::mutex mutexLockPrint;

void PrintJobId(uint32_t toPrint)
{
	std::lock_guard<std::mutex> lock(mutexLockPrint);

	std::cout << toPrint << std::endl;
}

void TestThread()
{
	std::chrono::high_resolution_clock saturationTimer;
	const auto startTime = saturationTimer.now();

	std::vector<ThreadPool::TaskFuture<void>> v;
	for (std::uint32_t i = 0u; i < 21u; ++i)
	{
		v.push_back(submitJob([](uint32_t a)
		{
			PrintJobId(a);
		}, i));
	}
	for (auto& item : v)
	{
		item.get();
	}
	const auto dt = saturationTimer.now() - startTime;

	std::cout << dt.count() << std::endl;
}


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