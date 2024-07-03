#include "bx/engine/modules/graphics/backend/vulkan/cmd_queue.hpp"

#include "bx/engine/core/macros.hpp"

#include "bx/engine/modules/graphics/backend/vulkan/device.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/physical_device.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/cmd_list.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/semaphore.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/fence.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/validation.hpp"

namespace Vk
{
    CmdQueue::CmdQueue(const std::shared_ptr<Device> device,
        const PhysicalDevice& physicalDevice, QueueType type)
        : device(device) {
        vkGetDeviceQueue(device->GetDevice(), physicalDevice.GraphicsFamily(), 0, &this->queue);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags =
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        if (type == QueueType::GRAPHICS)
            poolInfo.queueFamilyIndex = physicalDevice.GraphicsFamily();
        else if (type == QueueType::COMPUTE)
            poolInfo.queueFamilyIndex = physicalDevice.ComputeFamily();
        else
            poolInfo.queueFamilyIndex = physicalDevice.PresentFamily();

        BX_ASSERT(!vkCreateCommandPool(device->GetDevice(), &poolInfo, nullptr, &this->cmdPool),
            "Failed to create command pool.");
    }

    CmdQueue::~CmdQueue() {
        this->ProcessCmdLists(true);
        std::queue<std::shared_ptr<CmdList>>().swap(this->idleCmdLists);

        vkDestroyCommandPool(this->device->GetDevice(), this->cmdPool, nullptr);
    }

    void CmdQueue::SubmitCmdList(std::shared_ptr<CmdList> cmdList, std::shared_ptr<Fence> fence,
        const List<Semaphore*>& waitSemaphores,
        const List<VkPipelineStageFlags>& waitStages,
        const List<Semaphore*>& signalSemaphores) {
        cmdList->End();

        std::vector<VkSemaphore> vkWaitSemaphores;
        std::vector<VkSemaphore> vkSignalSemaphores;
        for (auto& waitSemaphore : waitSemaphores) {
            vkWaitSemaphores.push_back(waitSemaphore->GetSemaphore());
        }
        for (auto& signalSemaphore : signalSemaphores) {
            vkSignalSemaphores.push_back(signalSemaphore->GetSemaphore());
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vkWaitSemaphores.size());
        submitInfo.pWaitSemaphores = vkWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vkSignalSemaphores.size());
        submitInfo.pSignalSemaphores = vkSignalSemaphores.data();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdList->cmdBuffer;

        if (!fence)
            fence = std::make_shared<Fence>("submit cmd list", this->device);
        vkQueueSubmit(this->queue, 1, &submitInfo, fence->GetFence());

        this->busyCmdLists.push(InFlightCmdList{ fence, cmdList });
    }

    VkQueue CmdQueue::GetQueue() const {
        return this->queue;
    }

    VkCommandPool CmdQueue::GetCmdPool() const {
        return this->cmdPool;
    }

    void CmdQueue::ProcessCmdLists(bool wait) {
        while (!this->busyCmdLists.empty()) {
            auto busyCmdList = this->busyCmdLists.front();
            if (busyCmdList.fence->IsComplete()) {
                busyCmdList.cmdList->Reset();
                this->idleCmdLists.push(busyCmdList.cmdList);
                this->busyCmdLists.pop();
            }
            else if (!wait) {
                break;
            }
        }
    }

    std::shared_ptr<CmdList> CmdQueue::GetCmdList() {
        if (this->idleCmdLists.empty()) {
            auto cmdList = std::shared_ptr<CmdList>(new CmdList(this->device, *this));
            cmdList->Begin();
            return cmdList;
        }
        else {
            auto cmdList = this->idleCmdLists.front();
            this->idleCmdLists.pop();
            cmdList->Begin();
            return cmdList;
        }
    }
}