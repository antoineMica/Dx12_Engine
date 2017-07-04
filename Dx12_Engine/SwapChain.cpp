#include "SwapChain.h"
#include "DX12Base.h"
#include "CommandQueue.h"
#include "Win32Window.h"


SwapChain::SwapChain()
{
	frameCount_ = 0;
}

SwapChain::~SwapChain()
{
	pDxSwapChain_->Release();

}

void SwapChain::Initialize(uint32_t frameCount, uint32_t width, uint32_t height, CommandQueue * commandQueue, DX12Base * dxBase)
{
	frameCount_ = frameCount;
	renderWidth_ = width;
	renderHeight_ = height;
	pCommandQueue_ = commandQueue;
	pDxBase_ = dxBase;

	//Add Swapchain
	{
		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = frameCount_;
		swapChainDesc.Width = renderWidth_;
		swapChainDesc.Height = renderHeight_;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		IDXGISwapChain1* swapChain;

		ThrowIfFailed(pDxBase_->dxgiFactory_->CreateSwapChainForHwnd(
			pCommandQueue_->pDxQueue_,		// Swap chain needs the queue so that it can force a flush on it.
			gWindow->GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		// This sample does not support fullscreen transitions.
		ThrowIfFailed(pDxBase_->dxgiFactory_->MakeWindowAssociation(gWindow->GetHandle(), DXGI_MWA_NO_ALT_ENTER));


		HRESULT hres = swapChain->QueryInterface(__uuidof(pDxSwapChain_), (void**)&(pDxSwapChain_));
		ThrowIfFailed(hres);
		swapChain->Release();
	}
}


HRESULT SwapChain::Present(uint32_t syncInterval, uint32_t flags)
{
	return pDxSwapChain_->Present(syncInterval, flags);
}