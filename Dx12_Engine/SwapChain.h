#pragma once
#include "CoreHeader.h"
#include "DxEnums.h"

class DX12Base;

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	void Initialize(uint32_t  frameCount, uint32_t width, uint32_t height, std::shared_ptr<ID3D12CommandQueue> commandQueue, std::shared_ptr<DX12Base> dxBase);

	HRESULT Present(uint32_t syncInterval, uint32_t flags);

	uint32_t frameCount_;
	uint32_t renderWidth_;
	uint32_t renderHeight_;

	//owned
	std::shared_ptr<IDXGISwapChain4> dxSwapChain_;

	//owned by external (wrong)
	//instead add DX12Renderer ? or global renderer
	std::shared_ptr<ID3D12CommandQueue> commandQueue_;
	std::shared_ptr<DX12Base> dxBase_;
};


