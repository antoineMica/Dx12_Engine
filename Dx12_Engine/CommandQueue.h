#pragma once
#include "CoreHeader.h"

class DX12Base;
class GraphicsCommandList;
class CommandQueue
{
public:
	CommandQueue();
	~CommandQueue();

	void Initialize(DX12Base * pDxBase, D3D12_COMMAND_QUEUE_FLAGS mFlags = D3D12_COMMAND_QUEUE_FLAG_NONE ,D3D12_COMMAND_LIST_TYPE mType = D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_PRIORITY mPriority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, uint32_t nodeMask = 0);
	void ExecuteCommandList(GraphicsCommandList * pCmdlist);
	HRESULT Signal(ID3D12Fence * pFence, uint64_t& fenceValue);

	ID3D12CommandQueue * pDxQueue_;
	D3D12_COMMAND_QUEUE_DESC mQueueDesc_;
};

