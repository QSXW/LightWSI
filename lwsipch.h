#pragma once

#include "Layer/Device.h"
#include "Layer/DispatcheTable.h"
#include "Layer/Handle.h"
#include "Layer/Instance.h"
#include "Layer/PhysicalDevice.h"
#include "Layer/Queue.h"
#include "Layer/Surface.h"
#include "Layer/Swapchain.h"

#ifdef _WIN32
#include "WSI/Direct2D/Direct.h"
#include "WSI/Direct2D/Surface.h"
#include "WSI/Direct2D/Swapchain.h"
#include "WSI/Direct2D/Sync.h"
#endif
