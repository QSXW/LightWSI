#include "Instance.h"

#include <unordered_map>
#include <mutex>

namespace LightWSI
{

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

}
