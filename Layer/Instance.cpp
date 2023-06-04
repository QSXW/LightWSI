#include "Instance.h"

#include <unordered_map>
#include <mutex>

namespace LightWSI
{
namespace PrivateData
{

template <typename T>
static inline void *GetKey(T dispatchableObject)
{
	return *reinterpret_cast<void **>(dispatchableObject);
}

static std::mutex InstanceDataLock;
static std::unordered_map<void *, std::unique_ptr<Instance>> InstanceData;

Instance::Instance() :
    Handle{},
    disp{}
{

}

Instance::Instance(VkInstance instance, const VkInstanceCreateInfo *pCreateInfo, InstanceDispatchTable &disp) :
    Handle(instance),
    disp(disp)
{

}

void Instance::Set(VkInstance handle, const VkInstanceCreateInfo *pCreateInfo, InstanceDispatchTable &disp)
{
	auto instance = std::make_unique<Instance>(handle, pCreateInfo, disp);
	void *key = GetKey(handle);

	std::lock_guard lock(InstanceDataLock);
    InstanceData.insert({ key, std::move(instance) });
}

Instance *Instance::Get(VkInstance instance)
{
	std::lock_guard lock(InstanceDataLock);
	auto it = InstanceData.find(GetKey(instance));	
	return it != InstanceData.end() ? it->second.get() : nullptr;
}

static std::mutex DeviceDataLock;
static std::unordered_map<void *, std::unique_ptr<Device>> DeviceData;

Device::Device() :
    Handle{},
    disp{}
{

}

Device::Device(VkDevice device, const VkDeviceCreateInfo *pCreateInfo, DeviceDispatchTable &disp) :
    Handle(device),
    disp(disp)
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

void Device::Set(VkDevice handle, const VkDeviceCreateInfo* pCreateInfo, DeviceDispatchTable& disp)
{
	auto device = std::make_unique<Device>(handle, pCreateInfo, disp);
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

}
}
