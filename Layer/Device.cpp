#include "Instance.h"

#include <unordered_map>
#include <mutex>
#include <memory>

namespace LightWSI
{

static std::mutex DeviceDataLock;
static std::unordered_map<void *, std::unique_ptr<Device>> DeviceData;

Device::Device() :
    Handle{},
    disp{}
{

}

Device::Device(PhysicalDevice physicalDevice, VkDevice device, const VkDeviceCreateInfo *pCreateInfo, DeviceDispatchTable &disp) :
    Handle(device),
    physicalDevice(std::move(physicalDevice)),
    disp(disp),
    buffer{},
    bufferResource{buffer.data(), buffer.size()},
    allocator{&bufferResource}
{

}

void Device::GetPrivateData(VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t *pData)
{
    disp.GetPrivateDataEXT(handle, objectType, objectHandle, privateDataSlot, pData);
}

VkResult Device::SetPrivateData(VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data)
{
    return disp.SetPrivateDataEXT(handle, objectType, objectHandle, privateDataSlot, data);
}

Queue Device::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    VkQueue queue = VK_NULL_HANDLE;
    disp.GetDeviceQueue(handle, queueFamilyIndex, queueIndex, &queue);

    return { this, queue };
}

void Device::Set(PhysicalDevice physicalDevice, VkDevice handle, const VkDeviceCreateInfo* pCreateInfo, DeviceDispatchTable& disp)
{
    auto device = std::make_unique<Device>(std::move(physicalDevice), handle, pCreateInfo, disp);
    void *key = GetKey(handle);

    std::lock_guard lock(DeviceDataLock);
    DeviceData.insert({key, std::move(device)});
}

Device *Device::Get(VkDevice device)
{
    std::lock_guard lock(DeviceDataLock);
    auto it = DeviceData.find(GetKey(device));
    return it != DeviceData.end() ? it->second.get() : nullptr;
}

void Device::Remove(VkDevice device)
{
    std::lock_guard lock(DeviceDataLock);
    DeviceData.erase(GetKey(device));
}

}
