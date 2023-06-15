#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "LightWSI.h"

namespace LightWSI
{
namespace Direct2D
{

using Microsoft::WRL::ComPtr;

static inline DXGI_FORMAT CAST(VkFormat format)
{
#define CASE(f) case VK_FORMAT_ ##f: return DXGI_FORMAT_ ##f;
    switch (format)
    {
        CASE(R8G8B8A8_UNORM)
        CASE(B8G8R8A8_UNORM)
        default:
            return DXGI_FORMAT_UNKNOWN;
    }
#undef CASE
}

static inline VkResult CAST(HRESULT hr)
{
    switch (hr)
    {
        case DXGI_ERROR_DEVICE_REMOVED:
            return VK_ERROR_DEVICE_LOST;
        case E_OUTOFMEMORY:
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        default:
            return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
}

#define DX_CHECK(x) if (HRESULT hr = x; FAILED(hr)) { return CAST(hr); }

}
}
