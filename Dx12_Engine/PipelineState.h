#pragma once
#include "CoreHeader.h"

class DX12Base;

class PipelineState
{
public:
	PipelineState();
	~PipelineState();


	void Initialize(DX12Base * pDxBase, const D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC mPsoDesc_;
	ID3D12PipelineState * pDxPSO_;
};

