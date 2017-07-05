#pragma once
#include "CoreHeader.h"

class DX12Base;

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	void Initialize(DX12Base * pDxBase, std::vector<float> vertices, uint32_t strideInBytes);

	uint32_t mStrideInBytes_;

	D3D12_HEAP_PROPERTIES mHeapProps_;
	D3D12_RESOURCE_DESC mResourceDesc_;

	ID3D12Resource * pDxResource_;
	D3D12_VERTEX_BUFFER_VIEW mBufferView_;
};

