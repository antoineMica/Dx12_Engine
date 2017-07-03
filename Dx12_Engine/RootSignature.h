#pragma once
#include "CoreHeader.h"

class DX12Base;

class RootSignature
{
public:
	RootSignature();
	~RootSignature();

	void Initialize(DX12Base * pDxBase);


	ID3D12RootSignature * pDxRootSig_;
};

