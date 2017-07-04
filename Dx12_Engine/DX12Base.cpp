#include "DX12Base.h"



DX12Base::DX12Base()
{
	numOfGPUs_ = 0;
	mRTVDescriptorHeapSize_ = 0;
	//ASSERT(SUCCEEDED(hres));
}


DX12Base::~DX12Base()
{
	device_->Release();

	for (uint32_t i = 0; i < MAX_GPUS; i++)
		if (gpus_[i] != NULL)
			gpus_[i]->Release();

	dxgiFactory_->Release();

#if defined(_DEBUG)
	debugController_->Release();
#endif

}

void DX12Base::Initialize()
{
	//Create Debug Controller
	std::uint32_t dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(debugController_), (void**)&(debugController_))))
	{
		debugController_->EnableDebugLayer();

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(dxgiFactory_), (void**)&(dxgiFactory_));
	assert(SUCCEEDED(hr));

	D3D_FEATURE_LEVEL gpu_feature_levels[MAX_GPUS];
	for (uint32_t i = 0; i < MAX_GPUS; ++i) {
		gpu_feature_levels[i] = (D3D_FEATURE_LEVEL)0;
	}

	IDXGIAdapter1* adapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory_->EnumAdapters1(i, &adapter); ++i) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		// Make sure the adapter can support a D3D12 device
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(device_), NULL))) {
			HRESULT hres = adapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&(gpus_[numOfGPUs_]));
			if (SUCCEEDED(hres)) {
				gpu_feature_levels[numOfGPUs_] = D3D_FEATURE_LEVEL_12_1;
				++numOfGPUs_;
			}
			adapter->Release();
		}
		else if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(device_), NULL))) {
			HRESULT hres = adapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&(gpus_[numOfGPUs_]));
			if (SUCCEEDED(hres)) {
				gpu_feature_levels[numOfGPUs_] = D3D_FEATURE_LEVEL_12_0;
				++numOfGPUs_;
			}
			adapter->Release();
		}
	}
	assert(numOfGPUs_ > 0);

	D3D_FEATURE_LEVEL target_feature_level = D3D_FEATURE_LEVEL_12_1;
	for (uint32_t i = 0; i < numOfGPUs_; ++i) {
		if (gpu_feature_levels[i] == D3D_FEATURE_LEVEL_12_1) {
			activeGPU_ = gpus_[i];
			break;
		}
	}

	if (activeGPU_ == NULL) {
		for (uint32_t i = 0; i <numOfGPUs_; ++i) {
			if (gpu_feature_levels[i] == D3D_FEATURE_LEVEL_12_0) {
				activeGPU_ = gpus_[i];
				target_feature_level = D3D_FEATURE_LEVEL_12_0;
				break;
			}
		}
	}

	assert(activeGPU_ != NULL);


	// Load functions
	{
		HMODULE module = ::GetModuleHandle(TEXT("d3d12.dll"));
		fnD3D12CreateRootSignatureDeserializer
			= (PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER)GetProcAddress(module,
				"D3D12SerializeVersionedRootSignature");

		fnD3D12SerializeVersionedRootSignature
			= (PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE)GetProcAddress(module,
				"D3D12SerializeVersionedRootSignature");

		fnD3D12CreateVersionedRootSignatureDeserializer
			= (PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER)GetProcAddress(module,
				"D3D12CreateVersionedRootSignatureDeserializer");
	}

	if ((fnD3D12CreateRootSignatureDeserializer == NULL) ||
		(fnD3D12SerializeVersionedRootSignature == NULL) ||
		(fnD3D12CreateVersionedRootSignatureDeserializer == NULL))
	{
		target_feature_level = D3D_FEATURE_LEVEL_12_0;
	}


	HRESULT hres = D3D12CreateDevice(activeGPU_.get(), target_feature_level, __uuidof(device_), (void**)(&device_));

	mRTVDescriptorHeapSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}