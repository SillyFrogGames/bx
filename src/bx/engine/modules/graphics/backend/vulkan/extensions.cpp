#include "bx/engine/modules/graphics/backend/vulkan/extensions.hpp"

#include "bx/engine/containers/string.hpp"
#include "bx/engine/containers/hash_set.hpp"
#include "bx/engine/core/macros.hpp"

namespace Vk
{
    bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
            availableExtensions.data());

        HashSet<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        bool supported = requiredExtensions.empty();
        if (!supported) {
            BX_LOGW("Missing device extensions:");
            for (auto& extension : requiredExtensions) {
                BX_LOGW("%s", extension.c_str());
            }
        }

        return supported;
    }

    bool CheckDeviceRaytracingExtensionSupport(VkPhysicalDevice physicalDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
            availableExtensions.data());

        HashSet<String> requiredExtensions(deviceRaytracingExtensions.begin(),
            deviceRaytracingExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        bool supported = requiredExtensions.empty();
        if (!supported) {
            BX_LOGW("Missing device extensions:");
            for (auto& extension : requiredExtensions) {
                BX_LOGW("%s", extension.c_str());
            }
        }

        return supported;
    }
}