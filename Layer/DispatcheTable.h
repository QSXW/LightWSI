#pragma once

#include "LightWSI.h"

namespace LightWSI
{

template <class T, class PFN, class HandleType = VkInstance>
static inline T GetProcAddress(PFN pGetProcAddress, const char *pName, HandleType handle = VK_NULL_HANDLE)
{
    T func = (T) (pGetProcAddress(handle, pName));
    if (!func)
    {
        LWSI_LOG_WARNING("Failed to get address of %s", pName);
    }

    return func;
}
                                                             
#define INSTANCE_FUNCTION_LIST(FUNC)                         \
    /* Vulkan 1.0 */                                         \
    FUNC(GetInstanceProcAddr,                         true ) \
    FUNC(CreateInstance,                              true ) \
    FUNC(DestroyInstance,                             true ) \
    FUNC(GetPhysicalDeviceProperties,                 true ) \
    FUNC(GetPhysicalDeviceImageFormatProperties,      true ) \
    FUNC(EnumerateDeviceExtensionProperties,          true ) \
    FUNC(GetPhysicalDeviceMemoryProperties,           true ) \
    /* VK_KHR_surface */                                     \
    FUNC(DestroySurfaceKHR,                           false) \
    FUNC(GetPhysicalDeviceSurfaceCapabilitiesKHR,     false) \
    FUNC(GetPhysicalDeviceSurfaceFormatsKHR,          false) \
    FUNC(GetPhysicalDeviceSurfacePresentModesKHR,     false) \
    FUNC(GetPhysicalDeviceSurfaceSupportKHR,          false) \
    /* VK_EXT_Win32_surface */                               \
    FUNC(CreateWin32SurfaceKHR,                       false) \
    /* VK_EXT_headless_surface */                            \
    FUNC(CreateHeadlessSurfaceEXT,                    false) \
    /* VK_KHR_get_surface_capabilities2 */                   \
    FUNC(GetPhysicalDeviceSurfaceCapabilities2KHR,    false) \
    FUNC(GetPhysicalDeviceSurfaceFormats2KHR,         false) \
    /* VK_KHR_get_physical_device_properties2 or */          \
    /* 1.1 (without KHR suffix) */                           \
    FUNC(GetPhysicalDeviceImageFormatProperties2KHR,  false) \
    FUNC(GetPhysicalDeviceFormatProperties2KHR,       false) \
    FUNC(GetPhysicalDeviceFeatures2KHR,               false) \
    /* VK_KHR_device_group + VK_KHR_surface or */            \
    /* 1.1 with VK_KHR_swapchain */                          \
    FUNC(GetPhysicalDevicePresentRectanglesKHR,       false) \
    /* VK_KHR_external_fence_capabilities or */              \
    /* 1.1 (without KHR suffix) */                           \
    FUNC(GetPhysicalDeviceExternalFencePropertiesKHR, false)

struct InstanceDispatchTable
{
    VkResult Populate(VkInstance instance, PFN_vkGetInstanceProcAddr pGetInstanceProcessAddr)
    {
        bool ret = true;
#define FUNC(func, r) \
        func = GetProcAddress<PFN_vk##func>(pGetInstanceProcessAddr, "vk" #func, instance); \
        if (r)																			    \
        {																				    \
            ret |= func != nullptr;														    \
        }
        INSTANCE_FUNCTION_LIST(FUNC)
#undef FUNC
        return ret ? VK_SUCCESS : VK_ERROR_INITIALIZATION_FAILED;
    }

#define FUNC(x, r) PFN_vk##x x{};
    INSTANCE_FUNCTION_LIST(FUNC)
#undef FUNC
};

#define DEVICE_FUNCTION_LIST(FUNC)                     \
    /* Vulkan 1.0 */                                   \
    FUNC(GetDeviceProcAddr,                    true )  \
    FUNC(GetDeviceQueue,                       true )  \
    FUNC(DeviceWaitIdle,                       true )  \
    FUNC(QueueSubmit,                          true )  \
    FUNC(QueueWaitIdle,                        true )  \
    FUNC(CreateCommandPool,                    true )  \
    FUNC(DestroyCommandPool,                   true )  \
    FUNC(AllocateCommandBuffers,               true )  \
    FUNC(FreeCommandBuffers,                   true )  \
    FUNC(ResetCommandBuffer,                   true )  \
    FUNC(BeginCommandBuffer,                   true )  \
    FUNC(EndCommandBuffer,                     true )  \
    FUNC(CreateImage,                          true )  \
    FUNC(DestroyImage,                         true )  \
    FUNC(GetImageMemoryRequirements,           true )  \
    FUNC(BindImageMemory,                      true )  \
    FUNC(AllocateMemory,                       true )  \
    FUNC(FreeMemory,                           true )  \
    FUNC(CreateFence,                          true )  \
    FUNC(DestroyFence,                         true )  \
    FUNC(CreateSemaphore,                      true )  \
    FUNC(DestroySemaphore,                     true )  \
    FUNC(SignalSemaphore,                      true )  \
    FUNC(ResetFences,                          true )  \
    FUNC(WaitForFences,                        true )  \
    FUNC(DestroyDevice,                        true )  \
    FUNC(GetMemoryWin32HandlePropertiesKHR,    true )  \
    /* VK_EXT_private_data */                          \
    FUNC(CreatePrivateDataSlotEXT,             true )  \
    FUNC(DestroyPrivateDataSlotEXT,            true )  \
    FUNC(GetPrivateDataEXT,                    true )  \
    FUNC(SetPrivateDataEXT,                    true )  \
    /* VK_KHR_swapchain */                             \
    FUNC(CreateSwapchainKHR,                   false)  \
    FUNC(DestroySwapchainKHR,                  false)  \
    FUNC(GetSwapchainImagesKHR,                false)  \
    FUNC(AcquireNextImageKHR,                  false)  \
    FUNC(QueuePresentKHR,                      false)  \
    /* VK_KHR_device_group + VK_KHR_swapchain or */    \
    /* 1.1 with VK_KHR_swapchain */                    \
    FUNC(AcquireNextImage2KHR,                 false)  \
    /* VK_KHR_device_group + VK_KHR_surface or */      \
    /* 1.1 with VK_KHR_swapchain */                    \
    FUNC(GetDeviceGroupSurfacePresentModesKHR, false)  \
    FUNC(GetDeviceGroupPresentCapabilitiesKHR, false)  \
    /* VK_KHR_external_memory_fd */                    \
    FUNC(GetMemoryFdPropertiesKHR,             false)  \
    /* VK_KHR_bind_memory2 or */                       \
    /* 1.1 (without KHR suffix) */                     \
    FUNC(BindImageMemory2KHR,                  false)  \
    /* VK_KHR_external_fence_fd */                     \
    FUNC(GetFenceFdKHR,                        false)  \
    FUNC(ImportFenceFdKHR,                     false)  \
    /* VK_KHR_external_semaphore_fd */                 \
    FUNC(ImportSemaphoreFdKHR,                 false)

struct DeviceDispatchTable
{
	VkResult Populate(VkDevice device, PFN_vkGetDeviceProcAddr pGetDeviceProcessAddr)
	{
		bool ret = true;
#define FUNC(func, r)                                                                  \
	func = GetProcAddress<PFN_vk##func>(pGetDeviceProcessAddr, "vk" #func, device);    \
	if (r)                                                                             \
	{                                                                                  \
		ret |= func != nullptr;                                                        \
	}
	DEVICE_FUNCTION_LIST(FUNC)
#undef FUNC
		return ret ? VK_SUCCESS : VK_ERROR_INITIALIZATION_FAILED;
	}

#define FUNC(x, r) PFN_vk##x x{};
	DEVICE_FUNCTION_LIST(FUNC)
#undef FUNC
};

}
