#include "DX12Renderer.h"
#include "Win32Window.h"


DX12Renderer::DX12Renderer(float wWidth, float wHeight)
{
	frameIndex_ = 0;
	descriptorHeapSize_ = 0;

	windowWidth_ = wWidth;
	windowHeight_ = wHeight;
	useWarpDevice_ = false;
	numOfGPUs_ = 0;
}


DX12Renderer::~DX12Renderer()
{
}


void DX12Renderer::Initialize()
{
	std::uint32_t dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(debugController_), (void**)&(debugController_))))
	{
		debugController_->EnableDebugLayer();

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(dxgiFactory_), (void**)&(dxgiFactory_)));

	//also created device
	InitGPUS();


	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device_->CreateCommandQueue(&queueDesc, __uuidof(commandQueue_), (void**)&(commandQueue_)));
	
	//Add Swapchain
	{
		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = frameCount_;
		swapChainDesc.Width = windowWidth_;
		swapChainDesc.Height = windowHeight_;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		IDXGISwapChain1* swapChain;

		ThrowIfFailed(dxgiFactory_->CreateSwapChainForHwnd(
			commandQueue_.get(),		// Swap chain needs the queue so that it can force a flush on it.
			gWindow->GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		// This sample does not support fullscreen transitions.
		ThrowIfFailed(dxgiFactory_->MakeWindowAssociation(gWindow->GetHandle(), DXGI_MWA_NO_ALT_ENTER));


		HRESULT hres = swapChain->QueryInterface(__uuidof(swapChain_), (void**)&(swapChain_));
		ThrowIfFailed(hres);
		swapChain->Release();
	}
	
	frameIndex_ = swapChain_->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = frameCount_;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ThrowIfFailed(device_->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(descriptorHeap_), (void**)&(descriptorHeap_)));

		descriptorHeapSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		rtvHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

		// Create a RTV for each frame.
		for (UINT n = 0; n < frameCount_; n++)
		{
			ThrowIfFailed(swapChain_->GetBuffer(n, __uuidof(renderTargets_[n]), (void**)&(renderTargets_[n])));
			device_->CreateRenderTargetView(renderTargets_[n].get(), nullptr, rtvHandle);
			rtvHandle.ptr +=  descriptorHeapSize_;
		}
	}

	ThrowIfFailed(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(commandAllocator_), (void**)(&commandAllocator_)));


	// Create the command list.
	ThrowIfFailed(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.get(), nullptr, __uuidof(commandList_), (void**)(&commandList_)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(commandList_->Close());

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ThrowIfFailed(device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(rootSignature_), (void**)(&rootSignature_)));


	// Create the pipeline state, which includes compiling and loading shaders.
	{
		ID3DBlob * vertexShader;
		ID3DBlob* pixelShader;

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		ThrowIfFailed(D3DCompileFromFile(L"./shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		ThrowIfFailed(D3DCompileFromFile(L"./shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = rootSignature_.get();
		psoDesc.VS.BytecodeLength = vertexShader->GetBufferSize();
		psoDesc.VS.pShaderBytecode = vertexShader->GetBufferPointer();
		psoDesc.PS.BytecodeLength = pixelShader->GetBufferSize();
		psoDesc.PS.pShaderBytecode =pixelShader->GetBufferPointer();
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthClipEnable = TRUE;
		psoDesc.RasterizerState.MultisampleEnable = FALSE;
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;

		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(device_->CreateGraphicsPipelineState(&psoDesc, __uuidof(pipelineState_), (void**)(&pipelineState_)));
	}


	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * (windowWidth_/windowHeight_), 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f * (windowWidth_ / windowHeight_), 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * (windowWidth_ / windowHeight_), 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		D3D12_HEAP_PROPERTIES heapProps;
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC vertBufferDesc;
		vertBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertBufferDesc.Alignment = 0;
		vertBufferDesc.Width = vertexBufferSize;
		vertBufferDesc.Height = 1;
		vertBufferDesc.DepthOrArraySize = 1;
		vertBufferDesc.MipLevels = 1;
		vertBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		vertBufferDesc.SampleDesc.Count = 1;
		vertBufferDesc.SampleDesc.Quality = 0;
		vertBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		vertBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		ThrowIfFailed(device_->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&vertBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(vertexBuffer_), (void**)(&vertexBuffer_)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = 0;		// We do not intend to read from this resource on the CPU.
		readRange.End = 0;
		ThrowIfFailed(vertexBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		vertexBuffer_->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		vertexBufferview_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
		vertexBufferview_.StrideInBytes = sizeof(Vertex);
		vertexBufferview_.SizeInBytes = vertexBufferSize;
	}


	// Create synchronization objects.
	{
		ThrowIfFailed(device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(fence_), (void**)(&fence_)));
		fenceValue_ = 1;

		// Create an event handle to use for frame synchronization.
		fenceEventHandle_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEventHandle_ == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForPreviousFrame();
	}

}


void DX12Renderer::Update(float dt)
{

}


void DX12Renderer::Shutdown()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	WaitForPreviousFrame();

	CloseHandle(fenceEventHandle_);


	swapChain_->Release();
	device_->Release();
	for (uint32_t i = 0; i < MAX_GPUS; i++)
		if(gpus_[i] != NULL)
			gpus_[i]->Release();
	
	dxgiFactory_->Release();
	
	for (uint32_t i = 0; i < frameCount_; i++)
		renderTargets_[i]->Release();

	commandAllocator_->Release();
	commandQueue_->Release();
	descriptorHeap_->Release();
	//pipelineState_->Release();
	commandList_->Release();
	fence_->Release();

#if defined(_DEBUG)
	debugController_->Release();
#endif

}



void DX12Renderer::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { commandList_.get() };
	commandQueue_->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(swapChain_->Present(1, 0));

	WaitForPreviousFrame();

}

void DX12Renderer::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(commandAllocator_->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(commandList_->Reset(commandAllocator_.get(), pipelineState_.get()));


	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Height = windowHeight_;
	viewport.Width = windowWidth_;
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;

	D3D12_RECT scissor;
	scissor.top = 0;
	scissor.left= 0;
	scissor.bottom = windowHeight_;
	scissor.right = windowWidth_;

	commandList_->SetGraphicsRootSignature(rootSignature_.get());
	commandList_->RSSetViewports(1, &viewport);
	commandList_->RSSetScissorRects(1,&scissor);


	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = renderTargets_[frameIndex_].get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// Indicate that the back buffer will be used as a render target.
	commandList_->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle.ptr = descriptorHeap_->GetCPUDescriptorHandleForHeapStart().ptr + frameIndex_ * descriptorHeapSize_;
	commandList_->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	
	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->IASetVertexBuffers(0,1,&vertexBufferview_);
	commandList_->DrawInstanced(3,1,0,0);


	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// Indicate that the back buffer will now be used to present.
	commandList_->ResourceBarrier(1, &barrier);

	ThrowIfFailed(commandList_->Close());
}

void DX12Renderer::BeginFrame()
{
}

void DX12Renderer::EndFrame()
{
}

void DX12Renderer::WaitForPreviousFrame()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 fence = fenceValue_;
	ThrowIfFailed(commandQueue_->Signal(fence_.get(), fence));
	fenceValue_++;

	// Wait until the previous frame is finished.
	if (fence_->GetCompletedValue() < fence)
	{
		ThrowIfFailed(fence_->SetEventOnCompletion(fence, fenceEventHandle_));
		WaitForSingleObject(fenceEventHandle_, INFINITE);
	}

	frameIndex_ = swapChain_->GetCurrentBackBufferIndex();
}


void DX12Renderer::InitGPUS()
{
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
	//ASSERT(numOfGPUs_ > 0);

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

	//ASSERT(activeGPU != NULL);


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
	//ASSERT(SUCCEEDED(hres));

}