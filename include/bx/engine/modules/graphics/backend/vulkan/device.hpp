#pragma once

#include "vulkan_api.hpp"

namespace Vk
{
    class Instance;
	class PhysicalDevice;

    class Device {
    public:
        Device(std::shared_ptr<Instance> instance,
            const PhysicalDevice& physicalDevice, bool debug);
        ~Device();
        Device(const Device& other) = delete;
        Device& operator=(const Device& other) = delete;

        void WaitIdle() const;

        VkDevice GetDevice() const;
        VmaAllocator GetAllocator() const;

    private:
        VkDevice device;
        VmaAllocator allocator;

        const std::shared_ptr<Instance> instance;
    };
}