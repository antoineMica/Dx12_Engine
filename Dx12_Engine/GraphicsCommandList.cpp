#include "GraphicsCommandList.h"
#include "DX12Base.h"
#include "CommandAllocator.h"
#include "RenderTarget.h"

GraphicsCommandList::GraphicsCommandList()
{
	mListType_ = D3D12_COMMAND_LIST_TYPE_DIRECT;
}


GraphicsCommandList::~GraphicsCommandList()
{
	SAFE_RELEASE(pDxCmdList_);
}


void GraphicsCommandList::Initialize(D3D12_COMMAND_LIST_TYPE type, CommandAllocator * pAllocator, DX12Base * pDxBase)
{
	mListType_ = type;

	// Create the command list.
	assert(SUCCEEDED(pDxBase->device_->CreateCommandList(0, type, pAllocator->pDxAllocator_, nullptr, __uuidof(pDxCmdList_), (void**)(&pDxCmdList_))));

}


HRESULT GraphicsCommandList::Close()
{
	return pDxCmdList_->Close();
}


HRESULT GraphicsCommandList::Reset(CommandAllocator * pAllocator, ID3D12PipelineState * pPipelineState)
{
	return pDxCmdList_->Reset(pAllocator->pDxAllocator_, pPipelineState);
}

void GraphicsCommandList::SetRootSignature(ID3D12RootSignature * pRootSignature)
{
	pDxCmdList_->SetGraphicsRootSignature(pRootSignature);
}


void GraphicsCommandList::SetViewports(uint32_t mViewportCount, D3D12_VIEWPORT & mViewport)
{
	pDxCmdList_->RSSetViewports(mViewportCount, &mViewport);
}

void GraphicsCommandList::SetScissor(uint32_t mRectCount, D3D12_RECT & mScissor)
{
	pDxCmdList_->RSSetScissorRects(mRectCount, &mScissor);
}

void GraphicsCommandList::TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAGS mFlags, ID3D12Resource * pDxResource, D3D12_RESOURCE_STATES mStateBefore, D3D12_RESOURCE_STATES mStateAfter)
{
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = mFlags;
	barrier.Transition.pResource = pDxResource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = mStateBefore;
	barrier.Transition.StateAfter = mStateAfter;

	// Indicate that the back buffer will be used as a render target.
	pDxCmdList_->ResourceBarrier(1, &barrier);

}

void GraphicsCommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topo)
{
	pDxCmdList_->IASetPrimitiveTopology(topo);
}

void GraphicsCommandList::SetVertexBuffers(uint32_t mStartSlot, uint32_t mNumViews, D3D12_VERTEX_BUFFER_VIEW * pViews)
{
	pDxCmdList_->IASetVertexBuffers(mStartSlot, mNumViews, pViews);
}

void GraphicsCommandList::DrawInstanced(uint32_t mVertexCountPerInstance, uint32_t mInstanceCount, uint32_t mStartVertexLoocation, uint32_t mStartInstanceLocation)
{
	pDxCmdList_->DrawInstanced(mVertexCountPerInstance,mInstanceCount,mStartVertexLoocation,mStartInstanceLocation);
}

void GraphicsCommandList::SetRenderTarget(RenderTarget* pRenderTarget)
{
	pDxCmdList_->OMSetRenderTargets(1, &pRenderTarget->mRtvHandle_, FALSE, nullptr);
}

void GraphicsCommandList::ClearRenderTargetView(RenderTarget * pRenderTarget, const float clearColor[4])
{
	pDxCmdList_->ClearRenderTargetView(pRenderTarget->mRtvHandle_, clearColor,0,nullptr);
}