#pragma once

#include "Layer/Swapchain.h"
#include "Direct.h"
#include "Sync.h"
#include <array>

namespace LightWSI
{

class Device;
namespace Direct2D
{

class Surface;

struct SwapchainImage
{
    VkImage handle;
    VkDeviceMemory memory;
    HANDLE sharedHandle;
};

class Swapchain : public SuperSwapchain
{
public:
    Swapchain(Surface *surface, Device *device);

    virtual ~Swapchain();

    virtual size_t GetSize() override;

    virtual VkResult GetImages(Device *device, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages) override;

    virtual VkResult AcquireNextImage(Device *device, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex) override;

    virtual VkResult QueuePresent(Queue *queue, uint32_t imageIndex, VkSemaphore semaphore) override;

public:
    void Swap(Swapchain &other);

    VkResult Invalidate(const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator);
    
    VkResult CreateImages();

    void DestoryImages();

    bool operator!() const
    {
        return !handle.Get();
    }

protected:
    ComPtr<IDXGISwapChain4> handle;

    Surface *surface;

    Device *device;

    VkSwapchainCreateInfoKHR createInfo;

    std::array<SwapchainImage, 6> images;
    
    Fence fence;

    uint32_t syncInterval;

    uint32_t imageIndex = 0;
};

}
}
