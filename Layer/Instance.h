#pragma once

#include "LightWSI.h"
#include "Handle.h"
#include "DispatcheTable.h"

namespace LightWSI
{

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

    template <class T>
    static Instance *Get(T handle)
    {
        Get((VkInstance)handle);
    }

public:
    InstanceDispatchTable disp;
};

}
