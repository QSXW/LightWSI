#include "Queue.h"

namespace LightWSI
{

VkResult Queue::Submit(uint32_t submitCount, const VkSubmitInfo *pSubmitInfo, VkFence fence)
{
    return device->disp.QueueSubmit(handle, submitCount, pSubmitInfo, fence);
}

void Queue::WaitIdle()
{
    device->disp.QueueWaitIdle(handle);
}

}
