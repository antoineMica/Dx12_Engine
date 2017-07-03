#pragma once
#include "CoreHeader.h"
#include "DxEnums.h"

class DX12Base
{
public:
	DX12Base();
	void Initialize();
	~DX12Base();

	std::shared_ptr<ID3D12Device> device_;
	std::shared_ptr<IDXGIAdapter4> gpus_[MAX_GPUS];
	std::shared_ptr<IDXGIAdapter4> activeGPU_;
	std::shared_ptr<IDXGIFactory5> dxgiFactory_;

#if defined(_DEBUG)
	std::shared_ptr<ID3D12Debug> debugController_;
#endif

	uint32_t numOfGPUs_;
	uint32_t mRTVDescriptorHeapSize_;

	// Functions points for functions that need to be loaded
	PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER           fnD3D12CreateRootSignatureDeserializer = NULL;
	PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE           fnD3D12SerializeVersionedRootSignature = NULL;
	PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER fnD3D12CreateVersionedRootSignatureDeserializer = NULL;
};

