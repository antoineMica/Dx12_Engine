#pragma once
#include "CoreHeader.h"

class DX12Base;

class DescriptorHeap
{
public:
	DescriptorHeap();
	~DescriptorHeap();

	void Initialize(DX12Base *pDxBase , D3D12_DESCRIPTOR_HEAP_DESC &mDescHeap);

	uint32_t mHandleIncrementSize_;
	ID3D12DescriptorHeap * pDxHeap_;
	D3D12_DESCRIPTOR_HEAP_DESC mDescHeap_;
};

