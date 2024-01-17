#pragma once

#include "LightWSI.h"
#include "Swapchain.h"

namespace LightWSI
{

class Device;
class Surface
{
public:
	virtual ~Surface() = default;

	virtual VkResult GetCapatibility(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) = 0;

	virtual VkResult GetFormats(VkPhysicalDevice physicalDevice, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats) = 0;

	virtual VkResult GetPresentModes(VkPhysicalDevice physicalDevice, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes) = 0;

	virtual VkResult CreateSwapchain(Device *device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, SuperSwapchain **ppSwapchain) = 0;

#ifdef VK_USE_PLATFORM_WIN32_KHR
	static VkResult CreateWin32SurfaceKHR(Instance *instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, Surface **ppSurface);
#endif

	static void DestroySurface(Instance *instance, Surface *surface, const VkAllocationCallbacks *pAllocator);
};

using SuperSurface = Surface;

}
