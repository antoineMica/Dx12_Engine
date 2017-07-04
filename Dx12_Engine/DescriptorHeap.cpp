#include "DescriptorHeap.h"
#include "DX12Base.h"


DescriptorHeap::DescriptorHeap()
{
	//DEFAULT RTV desc
	// Describe and create a render target view (RTV) descriptor heap.
	mDescHeap_.NumDescriptors = 0;
	mDescHeap_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	mDescHeap_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDescHeap_.NodeMask = 0;
}


DescriptorHeap::~DescriptorHeap()
{
	SAFE_RELEASE(pDxHeap_);
}

void DescriptorHeap::Initialize(DX12Base* pDxBase, D3D12_DESCRIPTOR_HEAP_DESC &mDescHeap)
{
	mDescHeap_ = mDescHeap;


	ThrowIfFailed(pDxBase->device_->CreateDescriptorHeap(&mDescHeap_, __uuidof(pDxHeap_), (void**)&(pDxHeap_)));

	mHandleIncrementSize_ = pDxBase->device_->GetDescriptorHandleIncrementSize(mDescHeap_.Type);
}
