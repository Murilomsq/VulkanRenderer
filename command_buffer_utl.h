#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

VkCommandBuffer beginSingleTimeCommands(VkCommandPool cmdPool, VkDevice device);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue, VkDevice device, VkCommandPool cmdPool);