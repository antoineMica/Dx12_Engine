#include "PipelineState.h"
#include "DX12Base.h"


PipelineState::PipelineState()
{
}


PipelineState::~PipelineState()
{
	SAFE_RELEASE(pDxPSO_);

}


void PipelineState::Initialize(DX12Base * pDxBase, const D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc)
{

	mPsoDesc_ = psoDesc;

	ThrowIfFailed(pDxBase->device_->CreateGraphicsPipelineState(&psoDesc, __uuidof(pDxPSO_), (void**)(&pDxPSO_)));
}