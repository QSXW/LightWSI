#include "Surface.h"

namespace LightWSI
{

namespace Direct2D
{

Surface::Surface() :
    handle{ }
{

}

VkResult Surface::Init(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator)
{
	handle = pCreateInfo->hwnd;



	return VK_SUCCESS;
}

Surface::~Surface()
{

}

VkResult Surface::GetCapatibility(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
	RECT rect{};
	GetWindowRect(handle, &rect);
	uint32_t width  = (uint32_t)(rect.right - rect.left);
	uint32_t height = (uint32_t)(rect.bottom - rect.top);
	
	pSurfaceCapabilities->minImageCount           = 1;
	pSurfaceCapabilities->maxImageCount           = 6;
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

Swapchain *Surface::CreateSwapchain(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator)
{
	return nullptr;
}

}
}
