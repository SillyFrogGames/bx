#include "bx/engine/modules/graphics/backend/graphics_vulkan.hpp"

#include "bx/engine/core/file.hpp"
#include "bx/engine/core/profiler.hpp"
#include "bx/engine/core/memory.hpp"
#include "bx/engine/core/macros.hpp"

#include "bx/engine/modules/window.hpp"

#include "bx/engine/modules/graphics/backend/vulkan/instance.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/physical_device.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/device.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/fence.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/semaphore.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/render_pass.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/image.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/swapchain.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/framebuffer.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/cmd_queue.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/cmd_list.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/descriptor_pool.hpp"
using namespace Vk;

#ifdef BX_WINDOW_GLFW_BACKEND
#include "bx/engine/modules/window/backend/window_glfw.hpp"
#endif

constexpr bool ENABLE_VALIDATION =
#ifdef _DEBUG
true;
#else
false;
#endif

static std::shared_ptr<Instance> s_instance;
static std::unique_ptr<PhysicalDevice> s_physicalDevice;
static std::shared_ptr<Device> s_device;
static std::unique_ptr<CmdQueue> s_cmdQueue;
static std::unique_ptr<DescriptorPool> s_descriptorPool;
static std::unique_ptr<Swapchain> s_swapchain;

static std::shared_ptr<Fence> s_presentFence;
static std::shared_ptr<CmdList> s_cmdList;

bool Graphics::Initialize()
{
#ifdef BX_WINDOW_GLFW_BACKEND
    GLFWwindow* glfwWindow = WindowGLFW::GetWindowPtr();

    uint32_t extensionsCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

#else

    BX_LOGE("Window backend not supported!");
    return false;
#endif

    i32 width, height;
    Window::GetSize(&width, &height);

    s_instance = std::make_shared<Instance>((void*)glfwWindow, ENABLE_VALIDATION);
    s_physicalDevice = std::make_unique<PhysicalDevice>(*s_instance);
    s_device = std::make_shared<Device>(s_instance, *s_physicalDevice, ENABLE_VALIDATION);
    s_cmdQueue = std::make_unique<CmdQueue>(s_device, *s_physicalDevice, QueueType::GRAPHICS);
    s_descriptorPool = std::make_unique<DescriptorPool>(s_device);
    s_swapchain = std::make_unique<Swapchain>(static_cast<u32>(width), static_cast<u32>(height), *s_instance, s_device, *s_physicalDevice);

    /*std::shared_ptr<Fence> fence = std::make_shared<Fence>("my fence", device);
    std::shared_ptr<Semaphore> semaphore = std::make_shared<Semaphore>("my semaphore", device);

    std::shared_ptr<Image> image = std::make_shared<Image>("my image", device, *physicalDevice, 512, 512, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_FORMAT_R8G8B8A8_SRGB);
    std::shared_ptr<Image> depthImage = std::make_shared<Image>("my depth image", device, *physicalDevice, 512, 512, 1, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D24_UNORM_S8_UINT);
    std::shared_ptr<RenderPass> renderPass = std::make_shared<RenderPass>("my render pass", device, List<VkFormat>{VK_FORMAT_R8G8B8A8_SRGB}, Optional<VkFormat>::Some(VK_FORMAT_D24_UNORM_S8_UINT));
    std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>("my framebuffer", device, List<std::shared_ptr<Image>>{image, depthImage}, renderPass);*/

    return true;
}

void Graphics::Shutdown()
{
    s_device->WaitIdle();
}

void Graphics::Reload()
{
    // TODO: No implementation
}

void Graphics::NewFrame()
{
    // Recylce finished old cmd lists
    s_cmdQueue->ProcessCmdLists();

    if (Window::IsActive())
    {
        // Recreate swapchain, make sure the old swapchain has been destructed first using `reset()`
        if (Window::WasResized())
        {
            i32 width, height;
            Window::GetSize(&width, &height);

            s_swapchain.reset();
            s_swapchain = std::make_unique<Swapchain>(static_cast<u32>(width), static_cast<u32>(height), *s_instance, s_device, *s_physicalDevice);
        }

        // Wait for the next image and acquire a recycled present fence
        s_presentFence = s_swapchain->NextImage();

        // All cmds of the entire frame will be recorded into a single cmd list
        // This is because we designed the graphics module api to act like it's immediate
        s_cmdList = s_cmdQueue->GetCmdList();
    }
}

void Graphics::EndFrame()
{
    if (Window::IsActive())
    {
        // TODO: all rendering can happen before the image is available if we create a seperate present blit pipeline
        // This can also act as a hdr to sdr conversion and enable us to render in hdr

        // Swapchain present pass


        // Execute all rendering cmds when the image is available
        List<Semaphore*> waitSemaphores{ &s_swapchain->GetImageAvailableSemaphore() };
        List<VkPipelineStageFlags> presentWaitStages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        List<Semaphore*> presentSignalSemaphores{
            &s_swapchain->GetRenderFinishedSemaphore() };
        s_cmdQueue->SubmitCmdList(s_cmdList, s_presentFence, waitSemaphores, presentWaitStages,
            presentSignalSemaphores);

        // Present when rendering is finished, indicated by the `presentSignalSemaphores`
        s_swapchain->Present(*s_cmdQueue, *s_presentFence, presentSignalSemaphores);
    }

    s_cmdList.reset();
}

TextureFormat Graphics::GetColorBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

TextureFormat Graphics::GetDepthBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

GraphicsHandle Graphics::GetCurrentBackBufferRT()
{
    return INVALID_GRAPHICS_HANDLE;
}

GraphicsHandle Graphics::GetDepthBuffer()
{
    return INVALID_GRAPHICS_HANDLE;
}

void Graphics::SetRenderTarget(const GraphicsHandle renderTarget, const GraphicsHandle depthStencil) {}
void Graphics::ReadPixels(u32 x, u32 y, u32 w, u32 h, void* pixelData, const GraphicsHandle renderTarget) {}

void Graphics::SetViewport(const f32 viewport[4]) {}

void Graphics::ClearRenderTarget(const GraphicsHandle renderTarget, const f32 clearColor[4]) {}
void Graphics::ClearDepthStencil(const GraphicsHandle depthStencil, GraphicsClearFlags flags, f32 depth, i32 stencil) {}

GraphicsHandle Graphics::CreateShader(const ShaderInfo& info)
{
    return INVALID_GRAPHICS_HANDLE;
}

void Graphics::DestroyShader(const GraphicsHandle shader) {}

GraphicsHandle Graphics::CreateTexture(const TextureInfo& info)
{
    return INVALID_GRAPHICS_HANDLE;
}
GraphicsHandle Graphics::CreateTexture(const TextureInfo& info, const BufferData& data)
{
    return INVALID_GRAPHICS_HANDLE;
}
void Graphics::DestroyTexture(const GraphicsHandle texture) {}

GraphicsHandle Graphics::CreateResourceBinding(const ResourceBindingInfo& info)
{
    return INVALID_GRAPHICS_HANDLE;
}
void Graphics::DestroyResourceBinding(const GraphicsHandle resources) {}
void Graphics::BindResource(const GraphicsHandle resources, const char* name, GraphicsHandle resource) {}

GraphicsHandle Graphics::CreatePipeline(const PipelineInfo& info)
{
    return INVALID_GRAPHICS_HANDLE;
}
void Graphics::DestroyPipeline(const GraphicsHandle pipeline) {}
void Graphics::SetPipeline(const GraphicsHandle pipeline) {}
void Graphics::CommitResources(const GraphicsHandle pipeline, const GraphicsHandle resources) {}

GraphicsHandle Graphics::CreateBuffer(const BufferInfo& info)
{
    return INVALID_GRAPHICS_HANDLE;
}
GraphicsHandle Graphics::CreateBuffer(const BufferInfo& info, const BufferData& data)
{
    return INVALID_GRAPHICS_HANDLE;
}
void Graphics::DestroyBuffer(const GraphicsHandle buffer) {}
void Graphics::UpdateBuffer(const GraphicsHandle buffer, const BufferData& data) {}

void Graphics::SetVertexBuffers(i32 i, i32 count, const GraphicsHandle* pBuffers, const u64* offset) {}
void Graphics::SetIndexBuffer(const GraphicsHandle buffer, i32 i) {}

void Graphics::Draw(const DrawAttribs& attribs) {}
void Graphics::DrawIndexed(const DrawIndexedAttribs& attribs) {}



// TODO: obliterate this obomination
void Graphics::DebugDraw(const Mat4& viewProj, const DebugDrawAttribs& attribs, const List<DebugVertex>& vertices)
{
}

// TODO: this can go when there's a imgui render impl using the higher level graphics module api

#include <backends/imgui_impl_vulkan.h>
ImGui_ImplVulkan_InitInfo GraphicsVulkan::ImGuiInitInfo()
{
    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = s_instance->GetInstance();
    info.PhysicalDevice = s_physicalDevice->GetPhysicalDevice();
    info.Device = s_device->GetDevice();
    info.QueueFamily = s_physicalDevice->GraphicsFamily();
    info.Queue = s_cmdQueue->GetQueue();
    info.DescriptorPool = s_descriptorPool->GetPool();
    info.RenderPass = s_swapchain->GetRenderPass()->GetRenderPass(); // TODO: this breaks on resize
    info.MinImageCount = 2; // TODO: should this be properly queried from the swapchain?
    info.ImageCount = 2;
    info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.MinAllocationSize = 1024 * 1024;
    return info;
}