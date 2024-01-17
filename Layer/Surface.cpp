#include "Surface.h"
#include "Instance.h"

#ifdef _WIN32
#include "WSI/Direct2D/Surface.h"
#endif

namespace LightWSI
{

#ifdef VK_USE_PLATFORM_WIN32_KHR
VkResult Surface::CreateWin32SurfaceKHR(Instance *instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, Surface **ppSurface)
{
	Direct2D::Surface *surface = new Direct2D::Surface;
	VkResult ret = surface->Init(instance, pCreateInfo, pAllocator);
	if (ret != VK_SUCCESS)
	{
		return ret;
	}

	*ppSurface = surface;
	return VK_SUCCESS;
}
#endif

void Surface::DestroySurface(Instance *instance, Surface *surface, const VkAllocationCallbacks *pAllocator)
{
	if (surface)
	{
		delete surface;
	}
}

}
