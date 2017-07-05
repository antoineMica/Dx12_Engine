#include "VertexBuffer.h"
#include "DX12Base.h"


VertexBuffer::VertexBuffer()
{
}


VertexBuffer::~VertexBuffer()
{
	SAFE_RELEASE(pDxResource_);

}


void VertexBuffer::Initialize(DX12Base * pDxBase, std::vector<float> vertices, uint32_t strideInBytes)
{
	//default heap description for vert buffer
	mHeapProps_.Type = D3D12_HEAP_TYPE_UPLOAD;
	mHeapProps_.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	mHeapProps_.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	mHeapProps_.CreationNodeMask = 1;
	mHeapProps_.VisibleNodeMask = 1;

	mResourceDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	mResourceDesc_.Alignment = 0;
	mResourceDesc_.Width = vertices.size()* sizeof(float);
	mResourceDesc_.Height = 1;
	mResourceDesc_.DepthOrArraySize = 1;
	mResourceDesc_.MipLevels = 1;
	mResourceDesc_.Format = DXGI_FORMAT_UNKNOWN;
	mResourceDesc_.SampleDesc.Count = 1;
	mResourceDesc_.SampleDesc.Quality = 0;
	mResourceDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	mResourceDesc_.Flags = D3D12_RESOURCE_FLAG_NONE;


	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	assert(SUCCEEDED(pDxBase->device_->CreateCommittedResource(
		&mHeapProps_,
		D3D12_HEAP_FLAG_NONE,
		&mResourceDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		__uuidof(pDxResource_), (void**)(&pDxResource_))));



	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	D3D12_RANGE readRange;
	readRange.Begin = 0;		// We do not intend to read from this resource on the CPU.
	readRange.End = 0;
	ThrowIfFailed(pDxResource_->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, &vertices[0], vertices.size() * sizeof(float));
	pDxResource_->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	mBufferView_.BufferLocation = pDxResource_->GetGPUVirtualAddress();
	mBufferView_.StrideInBytes = strideInBytes;
	mBufferView_.SizeInBytes = vertices.size() * sizeof(float);
}
