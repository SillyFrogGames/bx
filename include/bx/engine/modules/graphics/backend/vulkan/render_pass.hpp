#pragma once

#include "bx/engine/core/guard.hpp"
#include "bx/engine/containers/optional.hpp"
#include "bx/engine/containers/list.hpp"

#include "vulkan_api.hpp"

#include <memory>

namespace Vk
{
    class Device;

    class RenderPass : NoCopy {
    public:
        RenderPass(std::shared_ptr<Device> device,
            const List<VkFormat>& colorFormats,
            const Optional<VkFormat>& depthFormat);
        ~RenderPass();

        VkRenderPass GetRenderPass() const;

    private:
        VkRenderPass renderPass;

        const std::shared_ptr<Device> device;
    };
}