#pragma once

#include "LightWSI.h"
#include "Swapchain.h"

namespace LightWSI
{

class Swapchain
{
public:
	virtual ~Swapchain() = default;
};

using SuperSwapchain = Swapchain;

}
