//#pragma once
//
//#include "bx/engine/core/guard.hpp"
//
//#include "vulkan_api.hpp"
//
//namespace Vk
//{
//    class Instance;
//    class Device;
//    class PhysicalDevice;
//
//    class Swapchain : NoCopy {
//    public:
//        Swapchain(const Window& window, const GraphicsInstance& instance,
//            std::shared_ptr<LogicalDevice> device, const PhysicalDevice& physicalDevice);
//        ~Swapchain();
//        Swapchain(const Swapchain& other) = delete;
//        Swapchain& operator=(const Swapchain& other) = delete;
//
//        const Framebuffer& GetCurrentFramebuffer() const;
//        const Image& GetCurrentImage() const;
//        uint32_t GetCurrentFrameIdx() const;
//        Semaphore& GetImageAvailableSemaphore();
//        Semaphore& GetRenderFinishedSemaphore();
//
//        std::shared_ptr<RenderPass> GetRenderPass();
//
//        std::shared_ptr<Fence> NextImage();
//        void Present(const CmdQueue& queue, const Fence& fence,
//            const std::vector<Semaphore*>& semaphores);
//
//        VkFormat Format() const;
//        Rect2D Extent() const;
//
//        const static uint32_t MAX_FRAMES_IN_FLIGHT = 2;
//
//    private:
//        VkSwapchainKHR swapChain;
//        VkSurfaceFormatKHR format;
//        VkPresentModeKHR presentMode;
//        VkExtent2D extent;
//
//        uint32_t imageCount;
//        std::vector<std::shared_ptr<Image>> images;
//        std::vector<Framebuffer> framebuffers;
//
//        uint32_t currentFrame;
//        uint32_t currentImage;
//        std::vector<Semaphore> imageAvailableSemaphores;
//        std::vector<Semaphore> renderFinishedSemaphores;
//        std::vector<std::shared_ptr<Fence>> inflightFences;
//
//        std::shared_ptr<RenderPass> renderPass;
//
//        const std::shared_ptr<LogicalDevice> device;
//    };
//}