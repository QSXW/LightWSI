#pragma once

#include "LightWSI.h"
#include "Handle.h"
#include "PhysicalDevice.h"
#include "DispatcheTable.h"

#include <iostream>
#include <array>
#include <memory_resource>

namespace LightWSI
{

class Queue;
class Device : public Handle<VkDevice>
{
public:
    Device();

    Device(PhysicalDevice physicalDevice, VkDevice device, const VkDeviceCreateInfo *pCreateInfo, DeviceDispatchTable &disp);

    void GetPrivateData(VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t *pData);

    VkResult SetPrivateData(VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data);

    Queue GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex);

public:
    void Swap(Device &&other)
    {
        std::swap(disp, other.disp);
    }

    template <class U, class T>
    U *SetPrivateData(VkObjectType objectType, T objectHandle, VkPrivateDataSlot privateDataSlot)
    {
        U *data = nullptr;
        GetPrivateData(objectType, (uint64_t) objectHandle, privateDataSlot, (uint64_t)&data);
    }

    template <class T, class U>
    VkResult SetPrivateData(VkObjectType objectType, T objectHandle, VkPrivateDataSlot privateDataSlot, U data)
    {
        SetPrivateData(objectType, (uint64_t) objectHandle, privateDataSlot, (uint64_t)data);
    }

public:
    static void Set(PhysicalDevice physicalDevice, VkDevice device, const VkDeviceCreateInfo *pCreateInfo, DeviceDispatchTable &disp);

    static Device *Get(VkDevice device);

    static void Remove(VkDevice device);

    template <class T>
    static Device *Get(T handle)
    {
        Get((VkDevice) handle);
    }

public:
    PhysicalDevice physicalDevice;

    DeviceDispatchTable disp;

    std::array<uint8_t, 1248> buffer;

    std::pmr::monotonic_buffer_resource bufferResource;

    std::pmr::polymorphic_allocator<uint8_t> allocator;
};

}
