#pragma once

#include "LightWSI.h"
#include "Handle.h"
#include "DispatcheTable.h"

namespace LightWSI
{

class PhysicalDevice : public Handle<VkPhysicalDevice>
{
public:
    PhysicalDevice(VkPhysicalDevice handle = VK_NULL_HANDLE) :
        Handle(handle)
    {

    }

    PhysicalDevice(const PhysicalDevice &other) :
        Handle{other.handle}
    {

    }
};

}
