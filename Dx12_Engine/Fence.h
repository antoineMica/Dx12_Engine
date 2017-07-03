#pragma once
#include "CoreHeader.h"

class DX12Base;

class Fence
{
public:
	Fence();
	~Fence();

	void Initialize(DX12Base * pDxBase);
	uint32_t NextFenceValue();
	void WaitForEventCompletion(uint64_t valueToWaitFor);


	HANDLE mEventHandle_;
	std::uint64_t mNextFenceValue_;
	ID3D12Fence * pDxFence_;
};

