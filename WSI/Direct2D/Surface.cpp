#include "Surface.h"
#include "Swapchain.h"
#include "Layer/Instance.h"
#include <mutex>

namespace LightWSI
{

namespace Direct2D
{

using PFN_CreateDXGIFactory2     = decltype(::CreateDXGIFactory2)*;
using PFN_D3D12CreateDevice      = decltype(::D3D12CreateDevice)*;
using PFN_D3D12GetDebugInterface = decltype(::D3D12GetDebugInterface)*;

PFN_CreateDXGIFactory2     CreateDXGIFactory2;
PFN_D3D12CreateDevice      D3D12CreateDevice;
#ifdef _DEBUG
PFN_D3D12GetDebugInterface D3D12GetDebugInterface;
#endif

static VkResult Load()
{
	auto dxgi = LoadLibraryA("dxgi.dll");
	if (!dxgi)
	{
		LWSI_LOG_ERROR("Failed to load dxgi.dll");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	auto d3d12 = LoadLibraryA("d3d12.dll");
	if (!d3d12)
	{
		LWSI_LOG_ERROR("Failed to load d3d12.dll");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

    CreateDXGIFactory2 = (PFN_CreateDXGIFactory2)GetProcAddress(dxgi, "CreateDXGIFactory2");
	if (!CreateDXGIFactory2)
	{
		LWSI_LOG_ERROR("Failed to load CreateDXGIFactory2");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	D3D12CreateDevice = (PFN_D3D12CreateDevice)GetProcAddress(d3d12, "D3D12CreateDevice");
	if (!D3D12CreateDevice)
	{
		LWSI_LOG_ERROR("Failed to load D3D12CreateDevice");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

#ifdef _DEBUG
	D3D12GetDebugInterface = (PFN_D3D12GetDebugInterface) GetProcAddress(d3d12, "D3D12GetDebugInterface");
	if (!D3D12GetDebugInterface)
	{
		LWSI_LOG_ERROR("Failed to load D3D12GetDebugInterface");
		return VK_ERROR_INITIALIZATION_FAILED;
	}
#endif

	return VK_SUCCESS;
}

Surface::Surface() :
    handle{ }
{

}

VkResult Surface::Init(Instance *_instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator)
{
	handle = pCreateInfo->hwnd;
	instance = _instance;

	VkResult ret = VK_SUCCESS;
	if ((ret = Load()) != VK_SUCCESS)
	{
		return ret;
	}

	uint32_t flags = 0;

#if _DEBUG
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();

		flags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory2(flags, IID_PPV_ARGS(dxgiFactory.GetAddressOf()))))
	{
		LWSI_LOG_ERROR("Failed to create dxgi factory!");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (FAILED(dxgiFactory->EnumAdapters1(0, adapter.GetAddressOf())))
	{
		LWSI_LOG_ERROR("Failed to load adapter!");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
	{
		LWSI_LOG_ERROR("Failed to create d3d12 device!");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	D3D12_COMMAND_QUEUE_DESC desc{
		.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
	    .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
		.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0,
	};
	if (FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue))))
	{
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	return ret;
}

Surface::~Surface()
{

}

VkResult Surface::GetCapatibility(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
	RECT rect{};
	if (!GetClientRect(handle, &rect))
	{
		return VK_ERROR_SURFACE_LOST_KHR;
	}

	uint32_t width  = (uint32_t)(rect.right - rect.left);
	uint32_t height = (uint32_t)(rect.bottom - rect.top);
	
	pSurfaceCapabilities->minImageCount           = 1;
	pSurfaceCapabilities->maxImageCount           = 2;
	pSurfaceCapabilities->currentExtent           = { width, height  };
	pSurfaceCapabilities->minImageExtent          = { 1, 1 };
	pSurfaceCapabilities->maxImageExtent          = {0xffffffff, 0xffffffff};
	pSurfaceCapabilities->maxImageArrayLayers     = 1;
	pSurfaceCapabilities->supportedTransforms     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    pSurfaceCapabilities->currentTransform        = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	pSurfaceCapabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	pSurfaceCapabilities->supportedUsageFlags     = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	return VK_SUCCESS;
}

VkResult Surface::GetFormats(VkPhysicalDevice physicalDevice, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats)
{
	VkFormat formats[] = {
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_FORMAT_B8G8R8A8_UNORM,
	};

	if (!pSurfaceFormats)
	{
		*pSurfaceFormatCount = LWSI_LENGTH(formats);
	}
	else
	{
		for (uint32_t i = 0; i < *pSurfaceFormatCount; i++)
		{
			pSurfaceFormats[i] = VkSurfaceFormatKHR{ .format = formats[i], .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}
	}

	return VK_SUCCESS;
}

VkResult Surface::GetPresentModes(VkPhysicalDevice physicalDevice, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes)
{
	VkPresentModeKHR presentModes[] = {
	    VK_PRESENT_MODE_IMMEDIATE_KHR,
	    VK_PRESENT_MODE_MAILBOX_KHR,
	    VK_PRESENT_MODE_FIFO_KHR,
	};

	if (!pPresentModes)
	{
		*pPresentModeCount = LWSI_LENGTH(presentModes);
	}
	else
	{
		for (uint32_t i = 0; i < *pPresentModeCount; i++)
		{
			pPresentModes[i] = presentModes[i];
		}
	}

	return VK_SUCCESS;
}

VkResult Surface::CreateSwapchain(Device *device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, SuperSwapchain **ppSwapchain)
{
	Swapchain *swapchain = new (device->allocator.allocate_bytes(sizeof(Swapchain))) Swapchain(this, device);
	if (pCreateInfo->oldSwapchain)
	{
		Swapchain *oldSwapchain = (Swapchain *)pCreateInfo->oldSwapchain;
		oldSwapchain->Swap(*swapchain);
	}

	VkResult ret = swapchain->Invalidate(pCreateInfo, pAllocator);
	if (ret == VK_SUCCESS)
	{
		*ppSwapchain = swapchain;
	}

	return ret;
}

HRESULT Surface::CreateSwapChain(const DXGI_SWAP_CHAIN_DESC1 *pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain)
{
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(queue.Get(), handle, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
	if (FAILED(hr))
	{
		return hr;
	}

	return dxgiFactory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
}

}
}
