#pragma once
#include "CoreHeader.h"
#include "DxEnums.h"

class DX12Base;

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	void Initialize(uint32_t  frameCount, uint32_t width, uint32_t height, ID3D12CommandQueue * commandQueue, DX12Base* dxBase);

	HRESULT Present(uint32_t syncInterval, uint32_t flags);

	uint32_t frameCount_;
	uint32_t renderWidth_;
	uint32_t renderHeight_;

	//owned
	IDXGISwapChain4 * dxSwapChain_;

	//owned by external (wrong)
	//instead add DX12Renderer ? or global renderer
	ID3D12CommandQueue * commandQueue_;
	DX12Base * dxBase_;
};


