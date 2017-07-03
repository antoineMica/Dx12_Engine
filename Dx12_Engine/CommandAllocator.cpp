#include "CommandAllocator.h"
#include "DX12Base.h"


CommandAllocator::CommandAllocator()
{
	//default value
	mAllocatorType_= D3D12_COMMAND_LIST_TYPE_DIRECT;

}


CommandAllocator::~CommandAllocator()
{
	SAFE_RELEASE(pDxAllocator_);
}


void CommandAllocator::Initialize(D3D12_COMMAND_LIST_TYPE  type, DX12Base * pDxBase)
{
	mAllocatorType_ = type;
	
	assert(SUCCEEDED(pDxBase->device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(pDxAllocator_), (void**)(&pDxAllocator_))));
}

HRESULT CommandAllocator::Reset()
{
	return pDxAllocator_->Reset();
}