#include "bx/engine/modules/graphics/backend/vulkan/validation.hpp"

#include "bx/engine/core/macros.hpp"

namespace Vk
{
    void CheckValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            BX_ASSERT(layerFound, "Validation layer '%s' is not supported.", layerName);
        }
    }

    VkBool32 VulkanDebugCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject, size_t location, int32_t msgCode,
        const char* pLayerPrefix, const char* pMsg, void* pUserData) {
        if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
            BX_LOGE("[%s] %s", pLayerPrefix, pMsg);
        else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
            BX_LOGW("[%s]  %s", pLayerPrefix, pMsg);
        else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
            BX_LOGI("[%s] %s", pLayerPrefix, pMsg);
        else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
            BX_LOGW("[%s] %s", pLayerPrefix, pMsg);
        else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
            BX_LOGI("[%s] %s", pLayerPrefix, pMsg);
        return 0;
    }
}