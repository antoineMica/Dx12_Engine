#include "Fence.h"
#include "DX12Base.h"


Fence::Fence()
{
	mNextFenceValue_ = 1;
}


Fence::~Fence()
{
	SAFE_RELEASE(pDxFence_);

	CloseHandle(mEventHandle_);
}

void Fence::Initialize(DX12Base * pDxBase)
{
	mNextFenceValue_ = 1;

	assert(SUCCEEDED(pDxBase->device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(pDxFence_), (void**)(&pDxFence_))));

	mEventHandle_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (mEventHandle_ == nullptr)
	{
		assert(SUCCEEDED(HRESULT_FROM_WIN32(GetLastError())));
	}

}

void Fence::WaitForEventCompletion(uint64_t valueToWaitFor)
{
	if (pDxFence_->GetCompletedValue() < valueToWaitFor)
	{
		assert(SUCCEEDED(pDxFence_->SetEventOnCompletion(valueToWaitFor, mEventHandle_)));
		WaitForSingleObject(mEventHandle_, INFINITE);
	}

}


uint32_t Fence::NextFenceValue()
{
	mNextFenceValue_++;
	return mNextFenceValue_;
}