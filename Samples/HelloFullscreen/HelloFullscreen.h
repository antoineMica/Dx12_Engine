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
class Shader;

class HelloFullscreen : public ISystem
{
public:
	HelloFullscreen(float wWidth, float wHeight);
	~HelloFullscreen();

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
	static const UINT TextureWidth = 256;
	static const UINT TextureHeight = 256;
	static const UINT TexturePixelSize = 4;	// The number of bytes used to represent a pixel in the texture.

	DX12Base * pDxBase_;
	SwapChain * pSwapChain_;
	RenderTarget * pRenderTarget_[frameCount_];
	CommandAllocator * pCommandAllocator_;
	GraphicsCommandList * pGraphicsCommandList_;
	CommandQueue * pCmdQueue_;
	DescriptorHeap * pRtvHeap_;
	DescriptorHeap * pSrvHeap_;
	PipelineState * pPipelineStateObject_;
	RootSignature * pRootSignature_;
	Shader * pShader_;
	VertexBuffer * pVertexBuffer_;
	Fence * pFence_;

	ID3D12Resource * pTexture_;

	D3D12_VIEWPORT mViewport_;
	D3D12_RECT mScissor_;


	std::uint32_t descriptorHeapSize_;
	std::uint32_t frameIndex_;

	float windowWidth_;
	float windowHeight_;

};
