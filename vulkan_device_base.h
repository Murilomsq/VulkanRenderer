#pragma once

#include <vulkan/vulkan.h>

class VulkanDeviceBase {
public:
    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VulkanDeviceBase(VkDevice device, VkPhysicalDevice physicalDevice) :
        device(device), 
        physicalDevice(physicalDevice)
        {}
};