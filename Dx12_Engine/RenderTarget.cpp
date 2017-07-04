#include "RenderTarget.h"
#include "SwapChain.h"
#include "DX12Base.h"
#include "DescriptorHeap.h"


RenderTarget::RenderTarget()
{
}


RenderTarget::~RenderTarget()
{
	pDxResource_->Release();
}

void RenderTarget::Initialize(DescriptorHeap * pDescriptorHeap)
{
}


void RenderTarget::InitializeFromSwapChain(DescriptorHeap * pDescriptorHeap, SwapChain * pSwapChain, DX12Base * pDxBase, uint32_t bufferId)
{
	//insert in given descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = pDescriptorHeap->pDxHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += pDxBase->mRTVDescriptorHeapSize_ * bufferId;
	mRtvHandle_ = rtvHandle;

	{
		//get swapchain buffer
		assert(SUCCEEDED(pSwapChain->pDxSwapChain_->GetBuffer(bufferId, __uuidof(pDxResource_), (void**)&(pDxResource_))));
		//add render target view in heap
		pDxBase->device_->CreateRenderTargetView(pDxResource_, nullptr, rtvHandle);
	}

}