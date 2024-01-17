#pragma once

#include "Direct.h"
#include <iostream>

namespace LightWSI
{
namespace Direct2D
{

class Fence
{
public:
    LWSI_SWAPPABLE(Fence)

public:
    Fence() :
	    handle{},
        hEvent{},
	    value{}
    {

    }

    Fence(ID3D12Device *device) :
	    Fence{}
    {
		if (SUCCEEDED(device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&handle))))
		{
			hEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
		}
    }

    ~Fence()
    {
        if (hEvent)
        {
			CloseHandle(hEvent);
        }

        handle.Reset();
    }

    HRESULT QueueSignal(ID3D12CommandQueue *queue)
    {
		return queue->Signal(handle.Get(), ++value);
    }

    HRESULT WaitIdle()
    {
		HRESULT hr = S_OK;
		uint64_t completion = handle->GetCompletedValue();
		if (completion < value)
		{
			hr = handle->SetEventOnCompletion(value, hEvent);
            if (SUCCEEDED(hr))
            {
				WaitForSingleObject(hEvent, INFINITE);
            }
		}

        return hr;
    }

    operator bool() const
    {
		return !!handle;
    }

    void Swap(Fence &other)
    {
        handle.Swap(other.handle);
        std::swap(hEvent, other.hEvent);
		std::swap(value,  other.value);
    }

protected:
	ComPtr<ID3D12Fence> handle;

    HANDLE hEvent;

    uint64_t value;
};

}
}
