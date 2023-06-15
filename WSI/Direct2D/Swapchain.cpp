#include "Swapchain.h"
#include "Surface.h"
#include "Layer/Instance.h"
#include <string>

namespace LightWSI
{

namespace Direct2D
{

Swapchain::Swapchain(Surface *surface, Device *device) :
    handle{},
    surface{surface},
    device{device},
    syncInterval{0},
    createInfo{},
    images{},
    fence{}
{

}

Swapchain::~Swapchain()
{
    if (fence)
    {
		fence.WaitIdle();
    }
    DestoryImages();
    handle.Reset();
}

size_t Swapchain::GetSize()
{
	return sizeof(*this);
}

VkResult Swapchain::GetImages(Device *device, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages)
{
    auto &swapchainImageCount = *pSwapchainImageCount;
    if (!pSwapchainImages)
    {
        swapchainImageCount = createInfo.minImageCount;
        return VK_SUCCESS;
    }

    if (swapchainImageCount > createInfo.minImageCount)
    {
        return VK_ERROR_VALIDATION_FAILED_EXT;
    }

    for (uint32_t i = 0; i < createInfo.minImageCount; i++)
    {
        pSwapchainImages[i] = images[i].handle;
    }

    return VK_SUCCESS;
}

VkResult Swapchain::AcquireNextImage(Device *device, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
    // *pImageIndex = handle->GetCurrentBackBufferIndex();
    
    *pImageIndex = imageIndex;
	imageIndex = (imageIndex + 1) % 2;

    Queue queue = device->GetQueue(0, 0);
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &semaphore;
    
    VkResult result = queue.Submit(1, &submitInfo, fence);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    queue.WaitIdle();
    return VK_SUCCESS;
}

VkResult Swapchain::QueuePresent(Queue *queue, uint32_t imageIndex, VkSemaphore semaphore)
{
    if (imageIndex != handle->GetCurrentBackBufferIndex())
    {
        return VK_ERROR_INCOMPATIBLE_DISPLAY_KHR;
    }
    VkPipelineStageFlags stageFlags = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores    = &semaphore;
    submitInfo.pWaitDstStageMask  = &stageFlags;

    VkResult result = queue->Submit(1, &submitInfo, VK_NULL_HANDLE);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    queue->WaitIdle();

    DXGI_PRESENT_PARAMETERS params{};
	DX_CHECK(handle->Present1(syncInterval, 0, &params));
    DX_CHECK(fence.QueueSignal(surface->queue.Get()));

    return VK_SUCCESS;
}

VkResult Swapchain::Invalidate(const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator)
{
    DXGI_FORMAT format = CAST(pCreateInfo->imageFormat);
    if (format == DXGI_FORMAT_UNKNOWN)
    {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    auto [width, height] = pCreateInfo->imageExtent;
    syncInterval = pCreateInfo->presentMode != VK_PRESENT_MODE_IMMEDIATE_KHR;

    if (handle)
    {
        DXGI_SWAP_CHAIN_DESC1 desc{};
        handle->GetDesc1(&desc);
            
        DX_CHECK(fence.WaitIdle());
        DestoryImages();
		DX_CHECK(handle->ResizeBuffers(pCreateInfo->minImageCount, width, height, format, desc.Flags));
    }
    else
    {
		fence = std::move(Fence(surface->device.Get()));

        DXGI_SWAP_CHAIN_DESC1 desc = {
            .Width        = width,
            .Height       = height,
            .Format       = format,
            .Stereo       = FALSE,
            .SampleDesc   = {.Count = 1, .Quality = 0},
            .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount  = pCreateInfo->minImageCount,
            .Scaling      = DXGI_SCALING_NONE,
		    .SwapEffect   = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
            .AlphaMode    = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
        };

        ComPtr<IDXGISwapChain1> swapchan1;
        if (FAILED(surface->CreateSwapChain(&desc, nullptr, nullptr, swapchan1.GetAddressOf())))
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        swapchan1.As(&handle);
    }

    imageIndex = 0;
    createInfo = *pCreateInfo;
    return CreateImages();
}

void Swapchain::DestoryImages()
{
    for (auto &image : images)
    {
        if (image.handle)
        {
			CloseHandle(image.sharedHandle);
            device->disp.FreeMemory(*device, image.memory, nullptr);
            device->disp.DestroyImage(*device, image.handle, nullptr);
            image.handle = VK_NULL_HANDLE;
            image.memory = VK_NULL_HANDLE;
            image.sharedHandle = nullptr;
        }
    }
}

VkResult Swapchain::CreateImages()
{
    for (uint32_t i = 0; i < createInfo.minImageCount; i++)
    {
        ComPtr<ID3D12Resource1> resource;
        if (FAILED(handle->GetBuffer(i, IID_PPV_ARGS(&resource))))
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
#ifdef _DEBUG
        resource->SetName((std::wstring(L"LightWSI::Swapchain::Render Target{") + std::to_wstring(i) + std::wstring(L"}")).c_str());
#endif
        D3D12_RESOURCE_DESC desc = resource->GetDesc();
        D3D12_HEAP_PROPERTIES properties{};
        D3D12_HEAP_FLAGS flags{};
        resource->GetHeapProperties(&properties, &flags);

        VkExternalMemoryImageCreateInfoKHR externalMemoryImageCreateInfo = {
            .sType       = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR,
            .pNext       = nullptr,
            .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT_KHR
        };

        VkImageCreateInfo imageCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext                 = &externalMemoryImageCreateInfo,
            .flags                 = 0,
            .imageType             = VK_IMAGE_TYPE_2D,
            .format                = createInfo.imageFormat,
            .extent                = { (uint32_t)desc.Width, (uint32_t)desc.Height, 1 },
            .mipLevels             = 1,
            .arrayLayers           = 1,
            .samples               = VK_SAMPLE_COUNT_1_BIT,
            .tiling                = VK_IMAGE_TILING_OPTIMAL,
            .usage                 = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr,
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VkResult result = device->disp.CreateImage(*device, &imageCreateInfo, nullptr, &images[i].handle);
        if (result != VK_SUCCESS)
        {
            LWSI_LOG_ERROR("Failed to create swapchain image!");
            return result;
        }

        if (FAILED(surface->device->CreateSharedHandle(
            resource.Get(),
            nullptr,
            GENERIC_ALL,
            std::to_wstring((uint64_t)images[i].handle).c_str(),
            &images[i].sharedHandle)))
        {
            LWSI_LOG_ERROR("Failed to create shared memory for image!");
        }

        VkMemoryWin32HandlePropertiesKHR memoryWin32HandleProperties{
            .sType          = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR,
            .pNext          = nullptr,
            .memoryTypeBits = 0,
        };
        result = device->disp.GetMemoryWin32HandlePropertiesKHR(
            *device,
            VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT,
            images[i].sharedHandle,
            &memoryWin32HandleProperties);
        if (result != VK_SUCCESS)
        {
            LWSI_LOG_ERROR("Failed to get Win32 memory handle properties!");
            return result;
        }

        VkPhysicalDeviceMemoryProperties memoryProperties{};
        surface->instance->disp.GetPhysicalDeviceMemoryProperties(device->physicalDevice, &memoryProperties);
        
        uint32_t memoryIndex = -1;
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (memoryWin32HandleProperties.memoryTypeBits == (1 << i))
            {
                if (memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                {
                    memoryIndex = i;
                }
                break;
            }
        }

        VkMemoryRequirements memoryRequirements{};
        device->disp.GetImageMemoryRequirements(*device, images[i].handle, &memoryRequirements);

        VkMemoryDedicatedAllocateInfoKHR dedicatedAllocateInfo{
            .sType  = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR,
            .pNext  = nullptr,
            .image  = images[i].handle,
            .buffer = VK_NULL_HANDLE,
        };

        VkImportMemoryWin32HandleInfoKHR importMemoryWind32HandleInfo{
            .sType      = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR,
            .pNext      = &dedicatedAllocateInfo,
            .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT_KHR,
            .handle     = images[i].sharedHandle,
            .name       = nullptr
        };

        VkMemoryAllocateInfo allocateInfo {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = &importMemoryWind32HandleInfo,
            .allocationSize  = memoryRequirements.size,
            .memoryTypeIndex = memoryIndex
        };

        result = device->disp.AllocateMemory(*device, &allocateInfo, nullptr, &images[i].memory);
        if (result != VK_SUCCESS)
        {
            LWSI_LOG_ERROR("Failed to allocated memory!");
            return result;
        }

        result = device->disp.BindImageMemory(*device, images[i].handle, images[i].memory, 0);
        if (result != VK_SUCCESS)
        {
            LWSI_LOG_ERROR("Failed to bind swapchain image memory!");
            return result;
        }
    }
    
    return VK_SUCCESS;
}

void Swapchain::Swap(Swapchain &other)
{
    images.swap(other.images);
    handle.Swap(other.handle);
	fence.Swap(other.fence);

    std::swap(surface,      other.surface     );
    std::swap(device,       other.device      );	
    std::swap(createInfo,   other.createInfo  );
    std::swap(syncInterval, other.syncInterval);
}

}

}
