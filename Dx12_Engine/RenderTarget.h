#pragma once
#include "CoreHeader.h"
#include "DxEnums.h"

class DescriptorHeap;
class SwapChain;
class DX12Base;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Initialize(DescriptorHeap * pDescriptorHeap);
	void InitializeFromSwapChain(DescriptorHeap * pDescriptorHeap, SwapChain * pSwapChain, DX12Base * pDxBase, uint32_t bufferId);

	//owned by RenderTarget
	ID3D12Resource * pDxResource_;

	//The descriptor heap owner
	DescriptorHeap * pDescriptorHeap_;

	//render target view handle to descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandle_;
};

