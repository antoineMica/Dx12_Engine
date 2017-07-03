#include "DescriptorHeap.h"
#include "DX12Base.h"


DescriptorHeap::DescriptorHeap()
{
}


DescriptorHeap::~DescriptorHeap()
{
	SAFE_RELEASE(pDxHeap_);
}

void DescriptorHeap::Initialize(DX12Base* pDxBase, uint32_t mNumDescriptors)
{

	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = mNumDescriptors;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(pDxBase->device_->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(pDxHeap_), (void**)&(pDxHeap_)));

	mHandleIncrementSize_ = pDxBase->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}
