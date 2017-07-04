#pragma once
#include "CoreHeader.h"
#include "DxEnums.h"

class DX12Base;
class CommandQueue;

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	void Initialize(uint32_t  frameCount, uint32_t width, uint32_t height, CommandQueue * commandQueue, DX12Base* dxBase);

	HRESULT Present(uint32_t syncInterval, uint32_t flags);

	uint32_t frameCount_;
	uint32_t renderWidth_;
	uint32_t renderHeight_;

	//owned
	IDXGISwapChain4 * pDxSwapChain_;

	//owned by external (wrong)
	//instead add DX12Renderer ? or global renderer
	CommandQueue * pCommandQueue_;
	DX12Base * pDxBase_;
};


