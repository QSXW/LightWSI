# Light Vulkan® Window System Integration Layer

## Introduction

This project is a Vulkan® layer which implements some of the Vulkan® window system
integration extensions such as `VK_KHR_swapchain`.

On Windows, the `vkCreateSwapchainKHR` have high overhead and will low down the FPS
when resizing the window, explicitly slower than the `D3D12 Swapchain ResizeBuffers`.

|Function Name|Total CPU \[unit, %\]|Self CPU \[unit, %\]|Module|Category|
|-|-|-|-|-|
| - \[External Call\] vulkan-1.dll!0x00007ffce575c065|872 \(66.26%\)|872 \(66.26%\)|vulkan-1||

## Comparison
The LightWSI (based on `DXGI Swapchain`) can keep in 144 FPS most of the time.
|WSI|FPS When Resizing|
| ----------- | ----------- |
|default|90-112|
|LightWSI|120-144|
