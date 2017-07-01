#pragma once
#include "ISystem.h"
#include <d3d12.h>  //directx 12 include
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory> //ptr stuff

enum {
	MAX_INSTANCE_EXTENSIONS = 1024,
	MAX_DEVICE_EXTENSIONS = 1024,
	MAX_GPUS = 4,
	MAX_DESCRIPTORS = 32,
	MAX_DESCRIPTOR_SETS = 8,				// for now only one descriptor set can be bound -> need to implement several descriptor sets
	MAX_RENDER_TARGET_ATTACHMENTS = 8,
	MAX_SUBMIT_CMDS = 20,					// max number of command lists / command buffers
	MAX_SUBMIT_WAIT_SEMAPHORES = 8,
	MAX_SUBMIT_SIGNAL_SEMAPHORES = 8,
	MAX_PRESENT_WAIT_SEMAPHORES = 8,
	MAX_VERTEX_BINDINGS = 15,
	MAX_VERTEX_ATTRIBS = 15,
	MAX_SEMANTIC_NAME_LENGTH = 128,
	MAX_DESCRIPTOR_ENTRIES = 8192,			// number of buffers / textures / samplers in the pointer array in the descriptor
	MAX_SAMPLER_DESCRIPTOR_ENTRIES = 2048,			// number of buffers / textures / samplers in the pointer array in the descriptor
	MAX_MIP_LEVELS = 0xFFFFFFFF,
	MAX_BUFFER_BINDINGS = 16,
	MAX_TEXTURE_BINDINGS = 32,
	MAX_SAMPLER_BINDINGS = 16
};



typedef struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
}Vertex;

class DX12Renderer : public ISystem
{
public:
	DX12Renderer(float wWidth, float wHeight);
	~DX12Renderer();

	void Initialize();
	void Update(float dt);
	void Shutdown();


	void Render();
	void BeginFrame();
	void EndFrame();
	void PopulateCommandList();
	void WaitForPreviousFrame();


	void InitGPUS();


private:
	static const uint32_t frameCount_ = 3;

	std::shared_ptr<IDXGISwapChain4> swapChain_;
	std::shared_ptr<ID3D12Device> device_;
	std::shared_ptr<IDXGIAdapter4> gpus_[MAX_GPUS];
	std::shared_ptr<IDXGIAdapter4> activeGPU_;
	std::shared_ptr<IDXGIFactory5> dxgiFactory_;
	std::shared_ptr<ID3D12Resource> renderTargets_[frameCount_];
	std::shared_ptr<ID3D12CommandAllocator> commandAllocator_;
	std::shared_ptr<ID3D12CommandQueue> commandQueue_;
	std::shared_ptr<ID3D12DescriptorHeap> descriptorHeap_;
	std::shared_ptr<ID3D12PipelineState> pipelineState_;
	std::shared_ptr<ID3D12RootSignature> rootSignature_;
	std::shared_ptr<ID3D12GraphicsCommandList> commandList_;

	std::shared_ptr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferview_;

	// Functions points for functions that need to be loaded
	PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER           fnD3D12CreateRootSignatureDeserializer = NULL;
	PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE           fnD3D12SerializeVersionedRootSignature = NULL;
	PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER fnD3D12CreateVersionedRootSignatureDeserializer = NULL;

#if defined(_DEBUG)
	std::shared_ptr<ID3D12Debug> debugController_;
#endif

	std::uint32_t descriptorHeapSize_;

	std::uint32_t frameIndex_;
	HANDLE fenceEventHandle_;
	std::shared_ptr<ID3D12Fence> fence_;
	std::uint64_t fenceValue_;

	uint32_t numOfGPUs_;

	float windowWidth_;
	float windowHeight_;

	bool useWarpDevice_;
};

