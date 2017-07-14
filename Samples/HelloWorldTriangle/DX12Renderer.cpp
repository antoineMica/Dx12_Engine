#include "DX12Renderer.h"
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

DX12Renderer::DX12Renderer(float wWidth, float wHeight)
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
	pDescriptorHeap_ = new DescriptorHeap();
	pPipelineStateObject_ = new PipelineState();
	pVertexBuffer_ = new VertexBuffer();
	pFence_ = new Fence();
	pRootSignature_ = new RootSignature();
	pShader_ = new Shader();
}


DX12Renderer::~DX12Renderer()
{
}


void DX12Renderer::Initialize()
{
	pDxBase_->Initialize();
	
	//function has default params (DIRECT cmd list and NORMAL priority)
	pCmdQueue_->Initialize(pDxBase_);
	pSwapChain_->Initialize(frameCount_,(uint32_t)windowWidth_, (uint32_t)windowHeight_, pCmdQueue_,pDxBase_);
	frameIndex_ = pSwapChain_->pDxSwapChain_->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC mDescHeap;

	mDescHeap.NumDescriptors = frameCount_;
	mDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	mDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDescHeap.NodeMask = 0;

	// Create descriptor heaps.
	pDescriptorHeap_->Initialize(pDxBase_, mDescHeap);
	descriptorHeapSize_ = pDescriptorHeap_->mHandleIncrementSize_;

	// Create frame resources.
	// Create a RTV for each frame.
	for (UINT n = 0; n < frameCount_; n++)
	{
		pRenderTarget_[n] = new RenderTarget();
		pRenderTarget_[n]->InitializeFromSwapChain(pDescriptorHeap_, pSwapChain_, pDxBase_, n);
	}
	
	pCommandAllocator_->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT, pDxBase_);
	pGraphicsCommandList_->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator_,pDxBase_);

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(pGraphicsCommandList_->Close());

	//Initialize basic root signature for now
	pRootSignature_->Initialize(pDxBase_);

	// Create the pipeline state, which includes compiling and loading shaders.
	{

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		assert(SUCCEEDED(pShader_->CompileVS("../../Shaders/HelloWorld.hlsl", "VSMain", "vs_5_0", compileFlags )));
		assert(SUCCEEDED(pShader_->CompilePS("../../Shaders/HelloWorld.hlsl", "PSMain", "ps_5_0", compileFlags )));

		
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2];
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[0].InputSlot = 0;
		inputElementDescs[0].AlignedByteOffset = 0;
		inputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementDescs[0].InstanceDataStepRate = 0;
		inputElementDescs[1].SemanticName = "COLOR";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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
		 0.0f, 0.25f * (windowWidth_/windowHeight_), 0.0f ,1.0f, 0.0f, 0.0f, 1.0f,
		 0.25f, -0.25f * (windowWidth_ / windowHeight_), 0.0f , 0.0f, 1.0f, 0.0f, 1.0f,
		 -0.25f, -0.25f * (windowWidth_ / windowHeight_), 0.0f , 0.0f, 0.0f, 1.0f, 1.0f 
		};

		pVertexBuffer_->Initialize(pDxBase_, triangleVertices, 7 * sizeof(float));
	}


	// Create synchronization objects.
	{
		pFence_->Initialize(pDxBase_);

		// Create an event handle to use for frame synchronization.nd 
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

	for (uint32_t i = 0; i < frameCount_; i++)
		SAFE_DELETE(pRenderTarget_[i]);

	SAFE_DELETE(pDxBase_);
	SAFE_DELETE(pSwapChain_);
	SAFE_DELETE(pCommandAllocator_);
	SAFE_DELETE(pGraphicsCommandList_);
	SAFE_DELETE(pCmdQueue_);
	SAFE_DELETE(pDescriptorHeap_);
	SAFE_DELETE(pPipelineStateObject_);
	SAFE_DELETE(pVertexBuffer_);
	SAFE_DELETE(pFence_);
	SAFE_DELETE(pRootSignature_);
	SAFE_DELETE(pShader_);

}



void DX12Renderer::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	pCmdQueue_->ExecuteCommandList(pGraphicsCommandList_);

	// Present the frame.
	assert(SUCCEEDED((pSwapChain_->Present(1,0))));

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
	assert(SUCCEEDED(pGraphicsCommandList_->Reset(pCommandAllocator_, pPipelineStateObject_)));


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

	pGraphicsCommandList_->SetRootSignature(pRootSignature_);
	pGraphicsCommandList_->SetViewports(1, viewport);
	pGraphicsCommandList_->SetScissor(1,scissor);

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
	const UINT64 currFenceValue = pFence_->mNextFenceValue_;
	pCmdQueue_->Signal(pFence_);

	//wait for previous frame
	pFence_->WaitForEventCompletion(currFenceValue);

	frameIndex_ = pSwapChain_->pDxSwapChain_->GetCurrentBackBufferIndex();
}