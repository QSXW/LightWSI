#include "Surface.h"
#include "Instance.h"
#include "WSI/Direct2D/Surface.h"

namespace LightWSI
{

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

void Surface::DestroySurface(Instance *instance, Surface *surface, const VkAllocationCallbacks *pAllocator)
{
	if (surface)
	{
		delete surface;
	}
}

}
