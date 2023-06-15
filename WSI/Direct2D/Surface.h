#pragma once

#include "Direct.h"
#include "Layer/Surface.h"
#include <memory_resource>
#include <array>

namespace LightWSI
{

class Instance;
class Device;
namespace Direct2D
{

class Surface : public SuperSurface
{
public:
	Surface();

	virtual ~Surface() override;

	virtual VkResult GetCapatibility(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) override;

	virtual VkResult GetFormats(VkPhysicalDevice physicalDevice, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats) override;

	virtual VkResult GetPresentModes(VkPhysicalDevice physicalDevice, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes) override;

	virtual VkResult CreateSwapchain(Device *device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, SuperSwapchain **ppSwapchain) override;

	VkResult Init(Instance *instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator);

	HRESULT CreateSwapChain(const DXGI_SWAP_CHAIN_DESC1 *pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain);

public:
	HWND handle;

	Instance *instance;

	ComPtr<IDXGIFactory4> dxgiFactory;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<ID3D12Device> device;

	ComPtr<ID3D12CommandQueue> queue;
};

}
}
