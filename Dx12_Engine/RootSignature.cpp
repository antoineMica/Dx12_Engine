#include "RootSignature.h"
#include "DX12Base.h"


RootSignature::RootSignature()
{
}


RootSignature::~RootSignature()
{
	SAFE_RELEASE(pDxRootSig_);
}

void RootSignature::Initialize(DX12Base * pDxBase, uint32_t staticSamplerCount)
{
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	if (staticSamplerCount > 0)
	{
		D3D12_DESCRIPTOR_RANGE1 ranges;
		ranges.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		ranges.NumDescriptors = 1;
		ranges.BaseShaderRegister = 0;
		ranges.RegisterSpace= 0;
		ranges.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		ranges.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;

		D3D12_ROOT_PARAMETER1 rootParameters;
		rootParameters.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters.DescriptorTable.NumDescriptorRanges = 1;
		rootParameters.DescriptorTable.pDescriptorRanges = &ranges;

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSigDesc;
		versionedRootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		versionedRootSigDesc.Desc_1_1.NumParameters = 1;
		versionedRootSigDesc.Desc_1_1.pParameters = &rootParameters;
		versionedRootSigDesc.Desc_1_1.NumStaticSamplers = 1;
		versionedRootSigDesc.Desc_1_1.pStaticSamplers = &sampler;
		versionedRootSigDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		assert(SUCCEEDED(D3D12SerializeVersionedRootSignature(&versionedRootSigDesc, &signature, &error)));
		assert(SUCCEEDED(pDxBase->device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(pDxRootSig_), (void**)(&pDxRootSig_))));
		
	}
	else
	{
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		assert(SUCCEEDED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)));
		assert(SUCCEEDED(pDxBase->device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(pDxRootSig_), (void**)(&pDxRootSig_))));
	}


}