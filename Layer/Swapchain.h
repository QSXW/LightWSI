#pragma once

#include "LightWSI.h"

namespace LightWSI
{

class Queue;
class Device;
class Swapchain
{
public:
    virtual ~Swapchain() = default;

    virtual VkResult GetImages(Device *device, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages) = 0;

    virtual VkResult AcquireNextImage(Device *device, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex) = 0;

    virtual VkResult QueuePresent(Queue *queue, uint32_t imageIndex, VkSemaphore semaphore) = 0;

    virtual size_t GetSize() = 0;
};

using SuperSwapchain = Swapchain;

}
