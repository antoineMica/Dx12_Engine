#include "CommandQueue.h"
#include "DX12Base.h"
#include "GraphicsCommandList.h"

CommandQueue::CommandQueue()
{
	mQueueDesc_.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	mQueueDesc_.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	mQueueDesc_.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	mQueueDesc_.NodeMask = 0;
}


CommandQueue::~CommandQueue()
{
	SAFE_RELEASE(pDxQueue_);
}


void CommandQueue::Initialize(DX12Base * pDxBase, D3D12_COMMAND_QUEUE_FLAGS mFlags, D3D12_COMMAND_LIST_TYPE mType, D3D12_COMMAND_QUEUE_PRIORITY mPriority, uint32_t nodeMask)
{
	mQueueDesc_.Flags = mFlags;
	mQueueDesc_.Type = mType;
	mQueueDesc_.Priority = mPriority;
	mQueueDesc_.NodeMask = nodeMask;

	ThrowIfFailed(pDxBase->device_->CreateCommandQueue(&mQueueDesc_, __uuidof(pDxQueue_), (void**)&(pDxQueue_)));
}

void CommandQueue::ExecuteCommandList(GraphicsCommandList * pCmdlist)
{
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { pCmdlist->pDxCmdList_ };
	pDxQueue_->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

}


HRESULT CommandQueue::Signal(ID3D12Fence * pFence, uint64_t& fenceValue)
{
	const uint64_t curFenceValue = fenceValue;
	HRESULT hr = pDxQueue_->Signal(pFence, curFenceValue);
	assert(SUCCEEDED(hr));
	fenceValue++;
	return hr;
}