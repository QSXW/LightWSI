#pragma once

#include "Layer/Surface.h"
#include <Windows.h>

namespace LightWSI
{

namespace Direct2D
{

class Surface : public SuperSurface
{
public:
	Surface();

	virtual ~Surface() override;

	virtual VkResult GetCapatibility(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) override;

	virtual Swapchain *CreateSwapchain(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator) override;

	VkResult Init(VkInstance instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator);

protected:
	HWND handle;
};

}
}
