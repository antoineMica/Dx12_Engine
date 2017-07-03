#pragma once
#include "CoreHeader.h"

class DX12Base;

class DescriptorHeap
{
public:
	DescriptorHeap();
	~DescriptorHeap();

	void Initialize(DX12Base *pDxBase , uint32_t mNumDescriptors);

	uint32_t mHandleIncrementSize_;
	ID3D12DescriptorHeap * pDxHeap_;
};

