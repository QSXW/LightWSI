#pragma once

#include "LightWSI.h"
#include "Swapchain.h"

namespace LightWSI
{

class Surface
{
public:
	virtual ~Surface() = default;

	virtual VkResult GetCapatibility(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) = 0;

	virtual Swapchain *CreateSwapchain(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator) = 0;

	static VkResult CreateWin32SurfaceKHR(VkInstance handle, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface);
};

using SuperSurface = Surface;

}
