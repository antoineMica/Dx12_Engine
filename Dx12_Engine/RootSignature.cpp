#include "RootSignature.h"
#include "DX12Base.h"


RootSignature::RootSignature()
{
}


RootSignature::~RootSignature()
{
	SAFE_RELEASE(pDxRootSig_);
}

void RootSignature::Initialize(DX12Base * pDxBase)
{
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ThrowIfFailed(pDxBase->device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(pDxRootSig_), (void**)(&pDxRootSig_)));


}