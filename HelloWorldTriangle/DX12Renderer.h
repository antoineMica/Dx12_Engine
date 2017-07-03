#pragma once
#include "ISystem.h"
#include "CoreHeader.h"

class DX12Base;
class SwapChain;
class RenderTarget;
class CommandAllocator;
class CommandQueue;
class DescriptorHeap;
class PipelineState;
class RootSignature;
class VertexBuffer;
class Fence;
class GraphicsCommandList;

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
	RootSignature * pRootSignature_;

	//contains buffer ressource, vertex buffer view
	VertexBuffer * pVertexBuffer_;
	Fence * pFence_;


	std::uint32_t descriptorHeapSize_;

	std::uint32_t frameIndex_;

	float windowWidth_;
	float windowHeight_;

	bool useWarpDevice_;
};

