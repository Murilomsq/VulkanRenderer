

#include <vulkan/vulkan.h>
#include "vulkan_device_base.h"

class Cubemap : public VulkanDeviceBase {
public:
    VkImage cubemapTextureImage;
    VkDeviceMemory cubemapTextureImageMemory;
    VkImageView cubemapTextureImageView;
    VkSampler cubemapSampler;

    Cubemap(VkDevice device, VkPhysicalDevice physicalDevice);

    void createCubemapImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void createCubemapImageView(VkImageView& cubemapTextureImageView, VkImage cubemapTextureImage);
};