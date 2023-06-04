#include "Surface.h"
#include "WSI/Direct2D/Surface.h"

namespace LightWSI
{

VkResult Surface::CreateWin32SurfaceKHR(VkInstance handle, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	auto surface = new Direct2D::Surface;
	VkResult ret = surface->Init(handle, pCreateInfo, pAllocator);
	if (ret != VK_SUCCESS)
	{
		return ret;
	}

	*pSurface = (VkSurfaceKHR)surface;
	return VK_SUCCESS;
}

}
