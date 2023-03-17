#ifndef IMGUI_HEADER_ONLY
#define IMGUI_HEADER_ONLY

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <stdexcept>
#include <mutex>

class ImGuiVulkan
{
private:
    VkDescriptorPool* _imGUIDescriptorPool;
    VkDevice* _device;
    std::mutex _mutex;

public:
    void init(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamily, VkQueue presentQueue,
        uint32_t imageCount, VkSampleCountFlagBits msaaSamples, VkRenderPass renderPass, VkDescriptorPool& imGUIDescriptorPool)
    {
        //Ensure only one thread executes initing at a time
        _mutex.lock();

        //Setup pointers to ImguiVulkan private attributes
        _imGUIDescriptorPool = &imGUIDescriptorPool;
        _device = &device;


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        ImGui::StyleColorsDark();


        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = device;
        init_info.QueueFamily = queueFamily;
        init_info.Queue = presentQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;


        //Creating descriptor pool for imgui
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        if (vkCreateDescriptorPool(device, &pool_info, nullptr, &imGUIDescriptorPool)) {
            throw std::runtime_error("failed to create DEAR IMGUI descriptor pool");
        }

        init_info.DescriptorPool = imGUIDescriptorPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;

        init_info.ImageCount = imageCount;
        init_info.MSAASamples = msaaSamples;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info, renderPass);
        _mutex.unlock();
    }
    void cleanup() {

        _mutex.lock(); //Ensure only one thread executes cleanup at a time
        vkDestroyDescriptorPool(*_device, *_imGUIDescriptorPool, nullptr);
        _mutex.unlock();
    }

};
#endif