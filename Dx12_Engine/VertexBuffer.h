#pragma once
#include "CoreHeader.h"

class DX12Base;

typedef struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
}Vertex;

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	void Initialize(DX12Base * pDxBase, Vertex* vertices,uint32_t mVertexCount);


	D3D12_HEAP_PROPERTIES mHeapProps_;
	D3D12_RESOURCE_DESC mResourceDesc_;

	ID3D12Resource * pDxResource_;
	D3D12_VERTEX_BUFFER_VIEW mBufferView_;
};

