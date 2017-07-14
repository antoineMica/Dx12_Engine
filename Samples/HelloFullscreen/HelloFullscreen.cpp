#include "HelloFullscreen.h"
#include "Win32Window.h"
#include "DX12Base.h"
#include "SwapChain.h"
#include "RenderTarget.h"
#include "CommandAllocator.h"
#include "GraphicsCommandList.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"
#include "PipelineState.h"
#include "VertexBuffer.h"
#include "Fence.h"
#include "RootSignature.h"
#include "Shader.h"


HelloFullscreen::HelloFullscreen(float wWidth, float wHeight)
{
	frameIndex_ = 0;
	descriptorHeapSize_ = 0;

	windowWidth_ = wWidth;
	windowHeight_ = wHeight;

	//create pointer
	pDxBase_ = new DX12Base();
	pSwapChain_ = new SwapChain();
	pCommandAllocator_ = new CommandAllocator();
	pGraphicsCommandList_ = new GraphicsCommandList();
	pCmdQueue_ = new CommandQueue();
	pRtvHeap_ = new DescriptorHeap();
	pSrvHeap_ = new DescriptorHeap();
	pPipelineStateObject_ = new PipelineState();
	pVertexBuffer_ = new VertexBuffer();
	pFence_ = new Fence();
	pRootSignature_ = new RootSignature();
	pShader_ = new Shader();
}


HelloFullscreen::~HelloFullscreen()
{
}


void HelloFullscreen::Initialize()
{
	pDxBase_->Initialize();
	
	//function has default params (DIRECT cmd list and NORMAL priority)
	pCmdQueue_->Initialize(pDxBase_);
	pSwapChain_->Initialize(frameCount_,(uint32_t)windowWidth_, (uint32_t)windowHeight_, pCmdQueue_,pDxBase_);
	frameIndex_ = pSwapChain_->pDxSwapChain_->GetCurrentBackBufferIndex();

	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frameCount_;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create descriptor heaps.
	pRtvHeap_->Initialize(pDxBase_, rtvHeapDesc);
	descriptorHeapSize_ = pRtvHeap_->mHandleIncrementSize_;


	// Describe and create a shader resource view (SRV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pSrvHeap_->Initialize(pDxBase_, srvHeapDesc);

	// Create frame resources.
	// Create a RTV for each frame.
	for (UINT n = 0; n < frameCount_; n++)
	{
		pRenderTarget_[n] = new RenderTarget();
		pRenderTarget_[n]->InitializeFromSwapChain(pRtvHeap_, pSwapChain_, pDxBase_, n);
	}
	
	pCommandAllocator_->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT, pDxBase_);
	pGraphicsCommandList_->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator_,pDxBase_);

	//Initialize basic root signature for now
	pRootSignature_->Initialize(pDxBase_,1);

	// Create the pipeline state, which includes compiling and loading shaders.
	{

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		assert(SUCCEEDED(pShader_->CompileVS("../Shaders/HelloTexture.hlsl", "VSMain", "vs_5_0", compileFlags )));
		assert(SUCCEEDED(pShader_->CompilePS("../Shaders/HelloTexture.hlsl", "PSMain", "ps_5_0", compileFlags )));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2];
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[0].InputSlot = 0;
		inputElementDescs[0].AlignedByteOffset = 0;
		inputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementDescs[0].InstanceDataStepRate = 0;
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].InputSlot = 0;
		inputElementDescs[1].AlignedByteOffset = 12;
		inputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementDescs[1].InstanceDataStepRate = 0;

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = pRootSignature_->pDxRootSig_;
		psoDesc.VS.BytecodeLength = pShader_->pVS_->GetBufferSize();
		psoDesc.VS.pShaderBytecode = pShader_->pVS_->GetBufferPointer();
		psoDesc.PS.BytecodeLength = pShader_->pPS_->GetBufferSize();
		psoDesc.PS.pShaderBytecode = pShader_->pPS_->GetBufferPointer();
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

		pPipelineStateObject_->Initialize(pDxBase_, psoDesc);
	}


	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		std::vector<float> triangleVertices =
		{
			0.0f, 0.25f * (windowWidth_ / windowHeight_), 0.0f ,0.5f, 0.0f,
			0.25f, -0.25f * (windowWidth_ / windowHeight_), 0.0f ,1.0f, 1.0f,
			-0.25f, -0.25f * (windowWidth_ / windowHeight_), 0.0f , 0.0f, 1.0f
		};

		pVertexBuffer_->Initialize(pDxBase_, triangleVertices, 5 * sizeof(float));
	}

	// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
	// the command list that references it has finished executing on the GPU.
	// We will flush the GPU at the end of this method to ensure the resource is not
	// prematurely destroyed.
	ID3D12Resource * textureUploadHeap;

	// Create the texture.
	{
		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = TextureWidth;
		textureDesc.Height = TextureHeight;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_HEAP_PROPERTIES defaultHeapProps;
		defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		defaultHeapProps.CreationNodeMask = 1;
		defaultHeapProps.VisibleNodeMask = 1;
		defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		ThrowIfFailed(pDxBase_->device_->CreateCommittedResource(
			&defaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			__uuidof(pTexture_), (void**)(&pTexture_)));

		UINT64 uploadBufferSize = 0;
		D3D12_RESOURCE_DESC Desc = pTexture_->GetDesc();
		ID3D12Device* pDevice;
		pTexture_->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
		pDevice->GetCopyableFootprints(&Desc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
		pDevice->Release();


		D3D12_HEAP_PROPERTIES uploadHeap;
		uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeap.CreationNodeMask = 1;
		uploadHeap.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = 0;
		uploadBufferDesc.Width = uploadBufferSize;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		// Create the GPU upload buffer.
		ThrowIfFailed(pDxBase_->device_->CreateCommittedResource(
			&uploadHeap,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap)));

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the Texture2D.
		std::vector<unsigned char> texture = GenerateTextureData();

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &texture[0];
		textureData.RowPitch = TextureWidth * TexturePixelSize;
		textureData.SlicePitch = textureData.RowPitch * TextureHeight;


		uint32_t textureByteSize = texture.size() * sizeof(unsigned char);

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
		layout.Offset = 0;
		layout.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		layout.Footprint.Width = TextureWidth;
		layout.Footprint.Height = TextureHeight;
		layout.Footprint.Depth = 1;
		layout.Footprint.RowPitch = TextureWidth * TexturePixelSize;

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = textureUploadHeap;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = layout;
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource = pTexture_;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		void * cpuMappedAddress = NULL;
		D3D12_RANGE read_range = { 0, 0 };
		HRESULT hres = textureUploadHeap->Map(0, &read_range, (void**)&(cpuMappedAddress));
		//Updates texture data using an upload buffer as intermediate (necessary) 
		//Update upload buffer with newly created texture data
		memcpy(cpuMappedAddress, &texture[0], textureByteSize);
		textureUploadHeap->Unmap(0, &read_range);

		pGraphicsCommandList_->pDxCmdList_->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);

		pGraphicsCommandList_->TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAG_NONE, pTexture_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		pDxBase_->device_->CreateShaderResourceView(pTexture_, &srvDesc, pSrvHeap_->pDxHeap_->GetCPUDescriptorHandleForHeapStart());
	}


	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(pGraphicsCommandList_->Close());

	pCmdQueue_->ExecuteCommandList(pGraphicsCommandList_);

	// Create synchronization objects.
	{
		pFence_->Initialize(pDxBase_);

		// Create an event handle to use for frame synchronization.nd 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForPreviousFrame();
	}
	textureUploadHeap->Release();



	mViewport_.TopLeftX = 0;
	mViewport_.TopLeftY = 0;
	mViewport_.Height = windowHeight_;
	mViewport_.Width = windowWidth_;
	mViewport_.MinDepth = D3D12_MIN_DEPTH;
	mViewport_.MaxDepth = D3D12_MAX_DEPTH;

	mScissor_.top = 0;
	mScissor_.left = 0;
	mScissor_.bottom = (LONG)windowHeight_;
	mScissor_.right = (LONG)windowWidth_;

}


void HelloFullscreen::Update(float dt)
{

}


void HelloFullscreen::Shutdown()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	WaitForPreviousFrame();

	for (uint32_t i = 0; i < frameCount_; i++)
		SAFE_DELETE(pRenderTarget_[i]);

	SAFE_DELETE(pDxBase_);
	SAFE_DELETE(pSwapChain_);
	SAFE_DELETE(pCommandAllocator_);
	SAFE_DELETE(pGraphicsCommandList_);
	SAFE_DELETE(pCmdQueue_);
	SAFE_DELETE(pRtvHeap_);
	SAFE_DELETE(pSrvHeap_);
	SAFE_DELETE(pPipelineStateObject_);
	SAFE_DELETE(pVertexBuffer_);
	SAFE_DELETE(pFence_);
	SAFE_DELETE(pRootSignature_);
	SAFE_DELETE(pShader_);
	
	SAFE_RELEASE(pTexture_);

}



void HelloFullscreen::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	pCmdQueue_->ExecuteCommandList(pGraphicsCommandList_);

	// Present the frame.
	assert(SUCCEEDED((pSwapChain_->Present(1,0))));

	WaitForPreviousFrame();

}

void HelloFullscreen::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	assert(SUCCEEDED(pCommandAllocator_->Reset()));

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	assert(SUCCEEDED(pGraphicsCommandList_->Reset(pCommandAllocator_, pPipelineStateObject_)));


	pGraphicsCommandList_->SetRootSignature(pRootSignature_);

	pGraphicsCommandList_->pDxCmdList_->SetDescriptorHeaps(1, &pSrvHeap_->pDxHeap_);
	pGraphicsCommandList_->pDxCmdList_->SetGraphicsRootDescriptorTable(0, pSrvHeap_->pDxHeap_->GetGPUDescriptorHandleForHeapStart());


	pGraphicsCommandList_->SetViewports(1, mViewport_);
	pGraphicsCommandList_->SetScissor(1, mScissor_);

	// Indicate that the back buffer will be used as a render target.
	pGraphicsCommandList_->TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAG_NONE, pRenderTarget_[frameIndex_]->pDxResource_, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	pGraphicsCommandList_->SetRenderTarget(pRenderTarget_[frameIndex_]);
	
	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	pGraphicsCommandList_->ClearRenderTargetView(pRenderTarget_[frameIndex_], clearColor);
	pGraphicsCommandList_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pGraphicsCommandList_->SetVertexBuffers(0,1,pVertexBuffer_);
	pGraphicsCommandList_->DrawInstanced(3,1,0,0);


	// Indicate that the back buffer will now be used to present.
	pGraphicsCommandList_->TransitionBarrier(D3D12_RESOURCE_BARRIER_FLAG_NONE, pRenderTarget_[frameIndex_]->pDxResource_, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


	assert(SUCCEEDED(pGraphicsCommandList_->Close()));
}

void HelloFullscreen::BeginFrame()
{
}

void HelloFullscreen::EndFrame()
{
}

void HelloFullscreen::WaitForPreviousFrame()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 currFenceValue = pFence_->mNextFenceValue_;
	pCmdQueue_->Signal(pFence_);

	//wait for previous frame
	pFence_->WaitForEventCompletion(currFenceValue);

	frameIndex_ = pSwapChain_->pDxSwapChain_->GetCurrentBackBufferIndex();
}
