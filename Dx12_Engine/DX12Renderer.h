#pragma once
#include "ISystem.h"
#include "CoreHeader.h"
#include "DX12Base.h"
#include "SwapChain.h"
#include "RenderTarget.h"
#include "CommandAllocator.h"
#include "GraphicsCommandList.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"
#include "PipelineState.h"
#include "VertexBuffer.h"

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

	DX12Base * dxBase_;
	SwapChain * swapChain_;
	RenderTarget * renderTargets_[frameCount_];
	CommandAllocator * pCommandAllocator_;
	GraphicsCommandList * pGraphicsCommandList_;
	CommandQueue * pCmdQueue_;
	DescriptorHeap * pDescriptorHeap_;
	PipelineState * pPipelineStateObject_;

	ID3D12RootSignature * rootSignature_;

	VertexBuffer * pVertexBuffer_;
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferview_;


	std::uint32_t descriptorHeapSize_;

	std::uint32_t frameIndex_;
	HANDLE fenceEventHandle_;
	std::uint64_t fenceValue_;
	ID3D12Fence * fence_;

	float windowWidth_;
	float windowHeight_;

	bool useWarpDevice_;
};

