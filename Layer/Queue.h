#pragma once

#include "LightWSI.h"
#include "Handle.h"
#include "Device.h"
#include "DispatcheTable.h"

namespace LightWSI
{

class Queue : public Handle<VkQueue, VK_NULL_HANDLE>
{
public:
    Queue(VkQueue handle = VK_NULL_HANDLE) :
        Handle(handle),
        device()
    {
        if (handle)
        {
            device = Device::Get((VkDevice)handle);
        }
    }

     Queue(Device *device, VkQueue handle) :
        Handle(handle),
        device(device)
    {

    }

    Queue(const Queue &other) :
        Handle{other.handle},
        device{other.device}
    {

    }

public:
    VkResult Submit(uint32_t submitCount, const VkSubmitInfo *pSubmitInfo, VkFence fence);
    
    void WaitIdle();

protected:
    Device *device;
};

}
