#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"



void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkCommandPool cmdPool,
	VkDevice device, VkQueue queue);
void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool cmdPool,
	VkDevice device, VkQueue queue);
void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int layer,
	VkCommandPool cmdPool, VkDevice device, VkQueue queue);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory,
	VkDevice device, VkPhysicalDevice physicalDevice);