#pragma once

#include "LightWSI.h"
#include "DispatcheTable.h"
#include <iostream>

#define SWAPPABLE(T) T(T &&other) : T{} { other.Swap(*this); } T &operator=(T &&other) { T(*this).Swap(other); return *this; }  T(const T &) = delete; T &operator=(const T &) = delete;

namespace LightWSI
{

namespace PrivateData
{

template <class T>
class Handle
{
public:
	SWAPPABLE(Handle)

public:
    Handle() :
	    handle{ VK_NULL_HANDLE }
    {

    }

    Handle(T handle) :
        handle{handle}
    {

    }

    operator T() const
    {
        return handle;
    }

    void Swap(Handle&& other)
    {
		std::swap(handle, other.handle);
    }

protected:
    T handle;
};

class Instance : public Handle<VkInstance>
{
public:
	Instance();

	Instance(VkInstance instance, const VkInstanceCreateInfo *pCreateInfo, InstanceDispatchTable &disp);

    void Swap(Instance &&other)
	{
		std::swap(disp, other.disp);
	}

public:
	static void Set(VkInstance instance, const VkInstanceCreateInfo *pCreateInfo, InstanceDispatchTable &disp);

	static Instance *Get(VkInstance instance);

public:
	InstanceDispatchTable disp;
};

class Device : public Handle<VkDevice>
{
public:
	Device();

	Device(VkDevice device, const VkDeviceCreateInfo *pCreateInfo, DeviceDispatchTable &disp);

	void Swap(Device &&other)
	{
		std::swap(disp, other.disp);
	}

	void GetPrivateData(VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t *pData);

	VkResult SetPrivateData(VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data);

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
	static void Set(VkDevice device, const VkDeviceCreateInfo *pCreateInfo, DeviceDispatchTable &disp);

	static Device *Get(VkDevice device);

public:
	DeviceDispatchTable disp;
};

}

}
