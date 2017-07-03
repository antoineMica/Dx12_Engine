#include "DX12Renderer.h"
#include "Win32Window.h"


DX12Renderer::DX12Renderer(float wWidth, float wHeight)
{
	frameIndex_ = 0;
	descriptorHeapSize_ = 0;

	windowWidth_ = wWidth;
	windowHeight_ = wHeight;
	useWarpDevice_ = false;

	//create pointer
	dxBase_ = new DX12Base();
	swapChain_ = new SwapChain();
	pCommandAllocator_ = new CommandAllocator();
	pGraphicsCommandList_ = new GraphicsCommandList();
	pCmdQueue_ = new CommandQueue();
	pDescriptorHeap_ = new DescriptorHeap();
	pPipelineStateObject_ = new PipelineState();
	pVertexBuffer_ = new VertexBuffer();
}


DX12Renderer::~DX12Renderer()
{
}


void DX12Renderer::Initialize()
{
	dxBase_->Initialize();
	
	//function has default params (DIRECT cmd list and NORMAL priority)
	pCmdQueue_->Initialize(dxBase_);
	swapChain_->Initialize(frameCount_,(uint32_t)windowWidth_, (uint32_t)windowHeight_, pCmdQueue_,dxBase_);
	frameIndex_ = swapChain_->dxSwapChain_->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		pDescriptorHeap_->Initialize(dxBase_, frameCount_);
		descriptorHeapSize_ = pDescriptorHeap_->mHandleIncrementSize_;
	}

	// Create frame resources.
	// Create a RTV for each frame.
	for (UINT n = 0; n < frameCount_; n++)
	{
		renderTargets_[n] = new RenderTarget();
		renderTargets_[n]->InitializeFromSwapChain(pDescriptorHeap_, swapChain_, dxBase_, n);
	}
	
	pCommandAllocator_->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT, dxBase_);
	pGraphicsCommandList_->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator_,dxBase_);

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(pGraphicsCommandList_->Close());

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ThrowIfFailed(dxBase_->device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(rootSignature_), (void**)(&rootSignature_)));


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
		psoDesc.pRootSignature = rootSignature_;
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

		pPipelineStateObject_->Initialize(dxBase_, psoDesc);
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

		pVertexBuffer_->Initialize(dxBase_, triangleVertices, 3);
	}


	// Create synchronization objects.
	{
		ThrowIfFailed(dxBase_->device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(fence_), (void**)(&fence_)));
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

	for (uint32_t i = 0; i < frameCount_; i++)
		SAFE_DELETE(renderTargets_[i]);

	SAFE_DELETE(dxBase_);
	SAFE_DELETE(swapChain_);
	SAFE_DELETE(pCommandAllocator_);
	SAFE_DELETE(pGraphicsCommandList_);
	SAFE_DELETE(pCmdQueue_);
	SAFE_DELETE(pDescriptorHeap_);
	SAFE_DELETE(pPipelineStateObject_);
	SAFE_DELETE(pVertexBuffer_);

	SAFE_RELEASE(rootSignature_);
	SAFE_RELEASE(fence_);

}



void DX12Renderer::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	pCmdQueue_->ExecuteCommandList(pGraphicsCommandList_);

	// Present the frame.
	assert(SUCCEEDED((swapChain_->Present(1,0))));

	WaitForPreviousFrame();

}

void DX12Renderer::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	assert(SUCCEEDED(pCommandAllocator_->Reset()));

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	assert(SUCCEEDED(pGraphicsCommandList_->Reset(pCommandAllocator_, pPipelineStateObject_->pDxPSO_)));


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
	scissor.bottom = (LONG)windowHeight_;
	scissor.right = (LONG)windowWidth_;

	pGraphicsCommandList_->SetRootSignature(rootSignature_);
	pGraphicsCommandList_->SetViewports(1, viewport);
	pGraphicsCommandList_->SetScissor(1,scissor);

	// Indicate that the back buffer will be used as a render target.
	pGraphicsCommandList_->TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAG_NONE, renderTargets_[frameIndex_]->pDxResource_, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	pGraphicsCommandList_->SetRenderTarget(renderTargets_[frameIndex_]);
	
	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	pGraphicsCommandList_->ClearRenderTargetView(renderTargets_[frameIndex_], clearColor);
	pGraphicsCommandList_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pGraphicsCommandList_->SetVertexBuffers(0,1,pVertexBuffer_);
	pGraphicsCommandList_->DrawInstanced(3,1,0,0);


	// Indicate that the back buffer will now be used to present.
	pGraphicsCommandList_->TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAG_NONE, renderTargets_[frameIndex_]->pDxResource_, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


	assert(SUCCEEDED(pGraphicsCommandList_->Close()));
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
	pCmdQueue_->Signal(fence_, fenceValue_);
	fenceValue_++;

	// Wait until the previous frame is finished.
	if (fence_->GetCompletedValue() < fence)
	{
		ThrowIfFailed(fence_->SetEventOnCompletion(fence, fenceEventHandle_));
		WaitForSingleObject(fenceEventHandle_, INFINITE);
	}

	frameIndex_ = swapChain_->dxSwapChain_->GetCurrentBackBufferIndex();
}