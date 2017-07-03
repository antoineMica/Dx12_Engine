#pragma once
#include "DxEnums.h"
#include "CoreHeader.h"

class DX12Base;

class CommandAllocator
{
public:
	CommandAllocator();
	~CommandAllocator();
	void Initialize(D3D12_COMMAND_LIST_TYPE  type, DX12Base * pDxBase);
	HRESULT Reset();

	//owns this
	ID3D12CommandAllocator * pDxAllocator_;

	D3D12_COMMAND_LIST_TYPE mAllocatorType_;

	//queue used with this allocator
};

