#pragma once
#include "DxEnums.h"
#include "CoreHeader.h"

class CommandAllocator;
class DX12Base;
class RenderTarget;
class VertexBuffer;

class GraphicsCommandList
{
public:
	GraphicsCommandList();
	~GraphicsCommandList();

	void Initialize(D3D12_COMMAND_LIST_TYPE type, CommandAllocator * pAllocator, DX12Base * pDxBase);


	HRESULT Close();
	HRESULT Reset(CommandAllocator * pAllocator, ID3D12PipelineState * pPipelineState);

	//commnand list functions
	void SetRootSignature(ID3D12RootSignature * pRootSignature);
	void SetViewports(uint32_t mViewportCount, D3D12_VIEWPORT & mViewport);
	void SetScissor(uint32_t mRectCount, D3D12_RECT & mScissor);
	void TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAGS mFlags, ID3D12Resource * pDxResource, D3D12_RESOURCE_STATES mStateBefore, D3D12_RESOURCE_STATES mStateAfter);
	void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY mTopo);
	void SetVertexBuffers(uint32_t mStartSlot, uint32_t mNumViews, VertexBuffer * pVertexBuffer);
	void DrawInstanced(uint32_t mVertexCountPerInstance, uint32_t mInstanceCount, uint32_t mStartVertexLoocation, uint32_t mStartInstanceLocation);
	void SetRenderTarget(RenderTarget * pRenderTarget);
	void ClearRenderTargetView(RenderTarget* pRenderTarget, const float clearColor[4]);


	//direct x command list
	ID3D12GraphicsCommandList * pDxCmdList_;
	//command list type {bundle, compute. direct}
	D3D12_COMMAND_LIST_TYPE mListType_;
};

