#pragma once
#include "ISystem.h"
#include "CoreHeader.h"
#include "DX12Base.h"
#include "SwapChain.h"


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

private:
	static const uint32_t frameCount_ = 3;

	std::shared_ptr<DX12Base> dxBase_;
	std::shared_ptr<SwapChain> swapChain_;

	std::shared_ptr<ID3D12Resource> renderTargets_[frameCount_];
	std::shared_ptr<ID3D12CommandAllocator> commandAllocator_;
	std::shared_ptr<ID3D12CommandQueue> commandQueue_;
	std::shared_ptr<ID3D12DescriptorHeap> descriptorHeap_;
	std::shared_ptr<ID3D12PipelineState> pipelineState_;
	std::shared_ptr<ID3D12RootSignature> rootSignature_;
	std::shared_ptr<ID3D12GraphicsCommandList> commandList_;

	std::shared_ptr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferview_;


	std::uint32_t descriptorHeapSize_;

	std::uint32_t frameIndex_;
	HANDLE fenceEventHandle_;
	std::shared_ptr<ID3D12Fence> fence_;
	std::uint64_t fenceValue_;

	float windowWidth_;
	float windowHeight_;

	bool useWarpDevice_;
};

