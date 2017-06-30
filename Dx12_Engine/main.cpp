#include "ThreadPool.hpp"
#include <iostream>

std::mutex mutexLockPrint;

void PrintJobId(uint32_t toPrint)
{
	std::lock_guard<std::mutex> lock(mutexLockPrint);

	std::cout << toPrint << std::endl;
}


int main()
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
	

	return 0;
}