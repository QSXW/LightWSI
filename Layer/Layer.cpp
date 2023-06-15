/**
 * Copyright (C) 2023, by Wu Jianhua (toqsxw@outlook.com)
 *
 * This library is distributed under the Apache-2.0 license.
 */


#include <vulkan/vk_layer.h>
#include <vulkan/vulkan.h>
#include <cassert>
#include <iostream>
#include <vector>

#include "LightWSI.h"
#include "DispatcheTable.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Queue.h"
#include "Surface.h"
#include "Swapchain.h"

namespace LightWSI
{

template <class R, class T>
VKAPI_ATTR R *GetChainInfo(const T *pCreateInfo, VkLayerFunction func)
{
    VkStructureType sType = VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
    if constexpr (std::is_same_v<T, VkDeviceCreateInfo>)
    {
        sType = VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO;
    }

    auto *chain = reinterpret_cast<const R *>(pCreateInfo->pNext);
    while (chain && !(chain->sType == sType && chain->function == func))
    {
        chain = reinterpret_cast<const R *>(chain->pNext);
    }

    return const_cast<R *>(chain);
}

#ifdef VK_USE_PLATFORM_WIN32_KHR
VkResult CreateWin32SurfaceKHR(VkInstance handle, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	Surface *surface = nullptr;
	VkResult result = Surface::CreateWin32SurfaceKHR(Instance::Get(handle), pCreateInfo, pAllocator, &surface);
    if (result == VK_SUCCESS)
    {
		*pSurface = (VkSurfaceKHR)surface;
    }

    return result;
}
#endif

void DestroySurfaceKHR(VkInstance handle, VkSurfaceKHR surface, const VkAllocationCallbacks *pAllocator)
{
	Surface::DestroySurface(Instance::Get(handle), (Surface *) surface, pAllocator);
}

VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32 *pSupported)
{
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR handle, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
    Surface *surface = (Surface *)handle;
    return surface->GetCapatibility(physicalDevice, pSurfaceCapabilities);
}

VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR handle, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats)
{
    Surface *surface = (Surface *)handle;
    return surface->GetFormats(physicalDevice, pSurfaceFormatCount, pSurfaceFormats);
}

VkResult GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR handle, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes)
{
    Surface *surface = (Surface *)handle;
    return surface->GetPresentModes(physicalDevice, pPresentModeCount, pPresentModes);
}

VkResult CreateSwapchainKHR(VkDevice handle, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain)
{
    Device *device = Device::Get(handle);
    Surface *surface = (Surface *)pCreateInfo->surface;

    Swapchain *swapchain = nullptr;
    VkResult ret = surface->CreateSwapchain(device, pCreateInfo, pAllocator, &swapchain);
    if (ret == VK_SUCCESS)
    {
        *pSwapchain = (VkSwapchainKHR) swapchain;
    }

    return ret;
}

void DestroySwapchainKHR(VkDevice device, VkSwapchainKHR handle, const VkAllocationCallbacks *pAllocator)
{
    Swapchain *swapchain = (Swapchain *)handle;
    swapchain->~Swapchain();
    
    Device *pDevice = Device::Get(device);
    pDevice->allocator.deallocate_bytes(swapchain, swapchain->GetSize());
}

VkResult GetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR handle, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages)
{
    Swapchain *swapchain = (Swapchain *) handle;
    return swapchain->GetImages(Device::Get(device), pSwapchainImageCount, pSwapchainImages);
}

VkResult AcquireNextImageKHR(VkDevice device, VkSwapchainKHR handle, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
    Swapchain *swapchain = (Swapchain *)handle;
    return swapchain->AcquireNextImage(Device::Get(device), timeout, semaphore, fence, pImageIndex);
}

VkResult AcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR *pAcquireInfo, uint32_t *pImageIndex)
{
    return VK_SUCCESS;
}

VkResult QueuePresentKHR(VkQueue handle, const VkPresentInfoKHR *pPresentInfo)
{
    Queue queue{handle};
    for (uint32_t i = 0; i < pPresentInfo->swapchainCount; i++)
    {
        Swapchain *swapchain = (Swapchain *)pPresentInfo->pSwapchains[i];
        VkResult result = swapchain->QueuePresent(&queue, pPresentInfo->pImageIndices[i], pPresentInfo->pWaitSemaphores[i]);
        if (pPresentInfo->pResults)
        {
            pPresentInfo->pResults[i] = result;
        }
        else if (result != VK_SUCCESS)
        {
            return result;
        }
    }

    return VK_SUCCESS;
}

VkResult CreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    VkLayerInstanceCreateInfo *layerLinkInfo = GetChainInfo<VkLayerInstanceCreateInfo>(pCreateInfo, VK_LAYER_LINK_INFO);
    VkLayerInstanceCreateInfo *layerDataCallback = GetChainInfo<VkLayerInstanceCreateInfo>(pCreateInfo, VK_LOADER_DATA_CALLBACK);

    if (!layerLinkInfo || !layerLinkInfo->u.pLayerInfo || !layerDataCallback)
    {
        LWSI_LOG_ERROR("Unexpected NULL pointer in layer initialization structures during vkCreateInstance");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    PFN_vkGetInstanceProcAddr pGetInstanceProcAddr = layerLinkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    if (!pGetInstanceProcAddr)
    {
        LWSI_LOG_ERROR("The vkGetInstanceProcAddr is nullptr!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    PFN_vkSetInstanceLoaderData pLoaderDataCallback  = layerDataCallback->u.pfnSetInstanceLoaderData;
    if (!pLoaderDataCallback)
    {
        LWSI_LOG_ERROR("The vkLoaderDataCallback is nullptr!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkInstanceCreateInfo createInfo = *pCreateInfo;
    layerLinkInfo->u.pLayerInfo = layerLinkInfo->u.pLayerInfo->pNext;
    
    auto pCreateInstance = GetProcAddress<PFN_vkCreateInstance>(pGetInstanceProcAddr, "vkCreateInstance");
    if (!pCreateInstance)
    {
        LWSI_LOG_ERROR("The vkCreateInstance is nullptr!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult ret = pCreateInstance(&createInfo, pAllocator, pInstance);
    if (ret != VK_SUCCESS)
    {
        LWSI_LOG_ERROR("Failed to create instance");
        if (*pInstance != VK_NULL_HANDLE)
        {
            auto pDestroyInstance = GetProcAddress<PFN_vkDestroyInstance>(pGetInstanceProcAddr, "vkDestroyInstance", *pInstance);
            pDestroyInstance(*pInstance, pAllocator);
        }
    }

    InstanceDispatchTable table{};
    ret = table.Populate(*pInstance, pGetInstanceProcAddr);
    if (ret != VK_SUCCESS)
    {
        return ret;
    }

    Instance::Set(*pInstance, &createInfo, table);

    return ret;
}

void DestroyInstance(VkInstance handle, const VkAllocationCallbacks *pAllocator)
{
    auto instance = Instance::Get(handle);
    if (instance)
    {
        instance->disp.DestroyInstance(handle, pAllocator);
    }
}

VkResult CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    VkLayerDeviceCreateInfo *layerLinkInfo      = GetChainInfo <VkLayerDeviceCreateInfo>(pCreateInfo, VK_LAYER_LINK_INFO);
    VkLayerDeviceCreateInfo *loaderDataCallback = GetChainInfo<VkLayerDeviceCreateInfo>(pCreateInfo, VK_LOADER_DATA_CALLBACK);

    if (!layerLinkInfo || !layerLinkInfo->u.pLayerInfo || !loaderDataCallback)
    {
        LWSI_LOG_ERROR("VK_LAYER_LINK_INFO is NULL!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    PFN_vkGetInstanceProcAddr pGetInstanceProcAddr = layerLinkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    if (!pGetInstanceProcAddr)
    {
        LWSI_LOG_ERROR("vkGetInstanceProcAddr is NULL!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    PFN_vkGetDeviceProcAddr pGetDeviceProcAddr = layerLinkInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    if (!pGetDeviceProcAddr)
    {
        LWSI_LOG_ERROR("vkGetDeviceProcAddr is NULL!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    PFN_vkSetDeviceLoaderData pLoaderCallback = loaderDataCallback->u.pfnSetDeviceLoaderData;
    if (!pLoaderCallback)
    {
        LWSI_LOG_ERROR("vkSetDeviceLoaderDat is NULL!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto pCreateDevice = GetProcAddress<PFN_vkCreateDevice>(pGetInstanceProcAddr, "vkCreateDevice");
    if (!pCreateDevice)
    {
        LWSI_LOG_ERROR("vkCreateDevice is NULL from the next vkGetInstanceProcAddr !");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    layerLinkInfo->u.pLayerInfo = layerLinkInfo->u.pLayerInfo->pNext;
    VkDeviceCreateInfo createInfo = *pCreateInfo;
    
    std::vector<const char *> deviceExntensions;
    deviceExntensions.reserve(pCreateInfo->enabledExtensionCount);
    for (size_t i = 0; i < pCreateInfo->enabledExtensionCount; i++)
    {
        deviceExntensions.emplace_back(pCreateInfo->ppEnabledExtensionNames[i]);
    }
    deviceExntensions.emplace_back("VK_EXT_private_data");
    deviceExntensions.emplace_back("VK_KHR_external_memory_win32");

    createInfo.enabledExtensionCount   = deviceExntensions.size();
    createInfo.ppEnabledExtensionNames = deviceExntensions.data();

    VkResult ret = pCreateDevice(physicalDevice, &createInfo, pAllocator, pDevice);
    if (ret != VK_SUCCESS)
    {
        if (ret == VK_ERROR_EXTENSION_NOT_PRESENT)
        {
            uint32_t count = 0;
            auto pEnumerateDeviceExtensionProperties = GetProcAddress<PFN_vkEnumerateDeviceExtensionProperties>(pGetInstanceProcAddr, "vkEnumerateDeviceExtensionProperties");
            VkResult ret = pEnumerateDeviceExtensionProperties(physicalDevice, "VK_EXT_private_data", &count, nullptr);
            if (ret != VK_SUCCESS)
            {
                LWSI_LOG_ERROR("VK_EXT_private_data doesn't support on your device!");
            }
        }
        return ret;
    }

    DeviceDispatchTable table{};
    ret = table.Populate(*pDevice, pGetDeviceProcAddr);
    if (ret != VK_SUCCESS)
    {
        return ret;
    }

    Device::Set(physicalDevice, *pDevice, &createInfo, table);

    return VK_SUCCESS;
}

void DestoryDevice(VkDevice handle, const VkAllocationCallbacks *pAllocator)
{
    auto device = Device::Get(handle);
    if (device)
    {
        device->disp.DestroyDevice(handle, pAllocator);
        Device::Remove(handle);
    }
}

#define LWSI_GET_PROC(func) if (!strcmp(pName, "vk" #func)) { return (PFN_vkVoidFunction)func; }
PFN_vkVoidFunction GetInstanceProcAddr(VkInstance handle, const char *pName)
{
    LWSI_GET_PROC(CreateWin32SurfaceKHR                  )
    LWSI_GET_PROC(DestroySurfaceKHR                      )
    LWSI_GET_PROC(GetPhysicalDeviceSurfaceSupportKHR     )
    LWSI_GET_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR)
    LWSI_GET_PROC(GetPhysicalDeviceSurfaceFormatsKHR     )
    LWSI_GET_PROC(GetPhysicalDeviceSurfacePresentModesKHR)

    auto instance = Instance::Get(handle);
    return instance->disp.GetInstanceProcAddr(handle, pName);
}

PFN_vkVoidFunction GetDeviceProcAddr(VkDevice handle, const char *pName)
{
    LWSI_GET_PROC(CreateSwapchainKHR                )
    LWSI_GET_PROC(DestroySwapchainKHR               )
    LWSI_GET_PROC(GetSwapchainImagesKHR             )
    LWSI_GET_PROC(AcquireNextImageKHR               )
    LWSI_GET_PROC(QueuePresentKHR                   )
    LWSI_GET_PROC(AcquireNextImage2KHR              )

    auto device = Device::Get(handle);
    return device->disp.GetDeviceProcAddr(handle, pName);
}
#undef LWSI_GET_PROC

}

LWSI_VKAPI_CALL(VkResult)
LightWSI_vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    return LightWSI::CreateInstance(pCreateInfo, pAllocator, pInstance);
}

LWSI_VKAPI_CALL(void)
LightWSI_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
    return LightWSI::DestroyInstance(instance, pAllocator);
}

LWSI_VKAPI_CALL(VkResult)
LightWSI_vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
   return LightWSI::CreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
}

LWSI_VKAPI_CALL(PFN_vkVoidFunction)
LightWSI_vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
   return LightWSI::GetDeviceProcAddr(device, pName);
}

#define LWSI_GET_PROC(func) if (!strcmp(pName, #func)) { return (PFN_vkVoidFunction)LightWSI_##func; }
LWSI_VKAPI_CALL(PFN_vkVoidFunction)
LightWSI_vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
   LWSI_GET_PROC(vkGetDeviceProcAddr  )
   LWSI_GET_PROC(vkGetInstanceProcAddr)
   LWSI_GET_PROC(vkCreateInstance     )
   LWSI_GET_PROC(vkDestroyInstance    )
   LWSI_GET_PROC(vkCreateDevice       )

   return LightWSI::GetInstanceProcAddr(instance, pName);
}

LWSI_VKAPI_CALL(VkResult)
LightWSI_vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface *pVersionStruct)
{
    assert(pVersionStruct);
    assert(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);
    assert(pVersionStruct->loaderLayerInterfaceVersion >= 2);

    pVersionStruct->loaderLayerInterfaceVersion = 2;
    pVersionStruct->pfnGetInstanceProcAddr      = LightWSI_vkGetInstanceProcAddr;
    pVersionStruct->pfnGetDeviceProcAddr        = LightWSI_vkGetDeviceProcAddr;
    pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;

    return VK_SUCCESS;
}

LWSI_VKAPI_CALL(VkResult)
LightWSI_EnumerateInstanceExtensionProperties(uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    return VK_SUCCESS;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,        // handle to DLL module
    DWORD fdwReason,           // reason for calling function
    LPVOID lpvReserved)        // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Initialize once for each new process.
            // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:

            if (lpvReserved != nullptr)
            {
                break;        // do not do cleanup if process termination scenario
            }

            // Perform any necessary cleanup.
            break;
    }
    return TRUE;        // Successful DLL_PROCESS_ATTACH.
}
