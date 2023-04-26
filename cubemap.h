#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <iostream>

#include "img_buff_memory_utl.h"
#include "vulkan_device_base.h"

class Cubemap : public VulkanDeviceBase {

public:
    uint32_t mipLevels;
    VkImage cubemapTextureImage;
    VkDeviceMemory cubemapTextureImageMemory;
    VkImageView cubemapTextureImageView;
    VkSampler cubemapSampler;


    Cubemap(VkDevice device, VkPhysicalDevice physicalDevice) : VulkanDeviceBase(device, physicalDevice) {}
    Cubemap(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool cmdPool, VkQueue queue) :
        VulkanDeviceBase(device, physicalDevice)
    {
        createCubemapTextureImage(cmdPool, queue);
        createCubemapImageView();
        createCubeMapSampler();
    }

    void cleanup() {
        vkDestroySampler(device, cubemapSampler, nullptr);
        vkDestroyImageView(device, cubemapTextureImageView, nullptr);
        vkFreeMemory(device, cubemapTextureImageMemory, nullptr);
        vkDestroyImage(device, cubemapTextureImage, nullptr);
    }

    void createCubemapImage(uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 6;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

        if (vkCreateImage(this->device, &imageInfo, nullptr, &cubemapTextureImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(this->device, cubemapTextureImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

        if (vkAllocateMemory(this->device, &allocInfo, nullptr, &cubemapTextureImageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(this->device, cubemapTextureImage, cubemapTextureImageMemory, 0);
    }

    void createCubeMapSampler() {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_TRUE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &cubemapSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void createCubemapTextureImage(VkCommandPool commandPool, VkQueue queue) {

        std::vector<int> texWidth(6), texHeight(6), texChannels(6);
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth[0], texHeight[0])))) + 1;

        stbi_set_flip_vertically_on_load(false);
        std::vector<stbi_uc*> pixels = {
            stbi_load("textures/bluecloudcubemap/bluecloud_lf.jpg", &texWidth[0], &texHeight[0], &texChannels[0], 4),
            stbi_load("textures/bluecloudcubemap/bluecloud_rt.jpg", &texWidth[1], &texHeight[1], &texChannels[1], 4),
            stbi_load("textures/bluecloudcubemap/bluecloud_up.png", &texWidth[2], &texHeight[2], &texChannels[2], 4),
            stbi_load("textures/bluecloudcubemap/bluecloud_dn.png", &texWidth[3], &texHeight[3], &texChannels[3], 4),
            stbi_load("textures/bluecloudcubemap/bluecloud_ft.jpg", &texWidth[4], &texHeight[4], &texChannels[4], 4),
            stbi_load("textures/bluecloudcubemap/bluecloud_bk.jpg", &texWidth[5], &texHeight[5], &texChannels[5], 4)
        };


        createCubemapImage(texWidth[0], texHeight[0], VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        for (size_t i = 0; i < 6; i++)
        {
            VkDeviceSize imageSize = texWidth[i] * texHeight[i] * 4;
            

            if (!pixels[i]) {
                throw std::runtime_error("failed to load texture image!");
            }

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer, stagingBufferMemory, device, physicalDevice);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels[i], static_cast<size_t>(imageSize));
            vkUnmapMemory(device, stagingBufferMemory);

            stbi_image_free(pixels[i]);



            transitionImageLayout(cubemapTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, commandPool,
                device, queue);
            copyBufferToImage(stagingBuffer, cubemapTextureImage, static_cast<uint32_t>(texWidth[i]), static_cast<uint32_t>(texHeight[i]), i, commandPool, device, queue);
            //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);

            transitionImageLayout(cubemapTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, commandPool, device, queue);

            generateMipmaps(cubemapTextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth[0], texHeight[0], mipLevels, commandPool, this->device, this->physicalDevice, queue);
        }
    }

    void createCubemapImageView() {
        cubemapTextureImageView = createImageView(this->device, cubemapTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_CUBE, 6);
    }
};