#pragma once

#include "Layer/Swapchain.h"
#include <Windows.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

namespace LightWSI
{

namespace Direct2D
{

using Microsoft::WRL::ComPtr;

class Swapchain : public SuperSwapchain
{
public:
	Swapchain();

	virtual ~Swapchain();

protected:
	ComPtr<IDXGISwapChain1> handle;
};

}
}
