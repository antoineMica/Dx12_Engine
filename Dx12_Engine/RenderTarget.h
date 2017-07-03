#pragma once
#include "CoreHeader.h"
#include "DxEnums.h"


class SwapChain;
class DX12Base;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Initialize(ID3D12DescriptorHeap * pDescriptorHeap);
	void InitializeFromSwapChain(ID3D12DescriptorHeap * pDescriptorHeap, SwapChain * pSwapChain, DX12Base * pDxBase, uint32_t bufferId);

	//owned by RenderTarget
	ID3D12Resource * pDxResource_;

	//The descriptor heap owner
	ID3D12DescriptorHeap * pDescriptorHeap_;

	//render target view handle to descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandle_;
};

