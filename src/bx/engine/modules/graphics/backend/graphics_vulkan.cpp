#include "bx/engine/modules/graphics/backend/graphics_vulkan.hpp"

#include "bx/engine/core/file.hpp"
#include "bx/engine/core/profiler.hpp"
#include "bx/engine/core/memory.hpp"
#include "bx/engine/core/macros.hpp"
#include "bx/engine/containers/array.hpp"

#include "bx/engine/modules/window.hpp"

#include "bx/engine/modules/graphics/backend/vulkan/instance.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/physical_device.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/device.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/fence.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/semaphore.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/render_pass.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/image.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/sampler.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/swapchain.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/framebuffer.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/cmd_queue.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/cmd_list.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/descriptor_pool.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/descriptor_set.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/descriptor_set_layout.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/shader.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/graphics_pipeline.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/rect2d.hpp"
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

static String PRESENT_VERT_SRC = R"""(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 fragTexCoord;

void main() {
    fragTexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(fragTexCoord * 2.0f + -1.0f, 0.0f, 1.0f);
    fragTexCoord.y = -fragTexCoord.y;
}
)""";

static String PRESENT_FRAG_SRC = R"""(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D colorImage;

void main() {
    vec3 color = texture(colorImage, fragTexCoord).rgb;
    outColor = vec4(color, 1.0);
}
)""";

static std::shared_ptr<Instance> s_instance;
static std::unique_ptr<PhysicalDevice> s_physicalDevice;
static std::shared_ptr<Device> s_device;
static std::unique_ptr<CmdQueue> s_cmdQueue;
static std::shared_ptr<DescriptorPool> s_descriptorPool;
static std::unique_ptr<Swapchain> s_swapchain;

static std::shared_ptr<Image> s_colorImage;
static std::shared_ptr<Image> s_depthImage;
static std::unique_ptr<Framebuffer> s_framebuffer;
static std::shared_ptr<RenderPass> s_renderPass;
static std::shared_ptr<Sampler> s_sampler;

std::shared_ptr<GraphicsPipeline> s_presentPipeline;
std::shared_ptr<DescriptorSetLayout> s_presentDescriptorSetLayout;
Array<std::shared_ptr<DescriptorSet>, Swapchain::MAX_FRAMES_IN_FLIGHT> s_presentDescriptorSets = { nullptr, nullptr };

static std::shared_ptr<Fence> s_presentFence;
static std::shared_ptr<CmdList> s_cmdList;

void BuildSwapchain()
{
    i32 width, height;
    Window::GetSize(&width, &height);

    s_swapchain.reset();
    s_swapchain = std::make_unique<Swapchain>(static_cast<u32>(width), static_cast<u32>(height), *s_instance, s_device, *s_physicalDevice);
}

void BuildRenderTargets()
{
    i32 width, height;
    Window::GetSize(&width, &height);

    s_colorImage = std::make_shared<Image>(
        "Color Image", s_device, *s_physicalDevice, width, height, 1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
        VK_IMAGE_USAGE_STORAGE_BIT,
        VK_FORMAT_R16G16B16A16_SFLOAT);
    s_depthImage = std::make_shared<Image>(
        "Depth Image", s_device, *s_physicalDevice, width, height, 1,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_FORMAT_D24_UNORM_S8_UINT);

    s_renderPass = std::make_shared<RenderPass>("Main Render Pass",
        s_device, List<VkFormat>{s_colorImage->Format()},
        Optional<VkFormat>::Some(s_depthImage->Format()));
    List<std::shared_ptr<Image>> images{ s_colorImage, s_depthImage };
    s_framebuffer = std::make_unique<Framebuffer>("Main Framebuffer", s_device, images, s_renderPass);
}

void BuildDescriptors()
{
    VkDescriptorSetLayoutBinding presentBinding0{};
    presentBinding0.binding = 0;
    presentBinding0.descriptorCount = 1;
    presentBinding0.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    presentBinding0.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    s_presentDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(
        "Present Descriptor Set Layout 0",
        s_device, List<VkDescriptorSetLayoutBinding>{presentBinding0});

    for (size_t i = 0; i < s_presentDescriptorSets.size(); i++) {
        s_presentDescriptorSets[i] = std::make_shared<DescriptorSet>(
            "Present Descriptor Set 0",
            s_device, s_descriptorPool, s_presentDescriptorSetLayout);
    }
}

void BuildPipelines()
{
    std::shared_ptr<Shader> presentVertexShader =
        std::make_shared<Shader>("present vert", s_device, VK_SHADER_STAGE_VERTEX_BIT, PRESENT_VERT_SRC);
    std::shared_ptr<Shader> presentFragmentShader =
        std::make_shared<Shader>("present frag", s_device, VK_SHADER_STAGE_FRAGMENT_BIT, PRESENT_FRAG_SRC);

    GraphicsPipelineInfo presentInfo{};
    presentInfo.ignoreDepth = true;
    presentInfo.inputVertices = false;
    std::vector<const Shader*> presentShaders = { presentVertexShader.get(),
                                                 presentFragmentShader.get() };
    s_presentPipeline = std::make_shared<GraphicsPipeline>(
        s_device, presentShaders, s_swapchain->GetRenderPass(),
        std::vector<std::shared_ptr<DescriptorSetLayout>>{s_presentDescriptorSetLayout},
        std::vector<PushConstantRange>{}, presentInfo);
}

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

    s_instance = std::make_shared<Instance>((void*)glfwWindow, ENABLE_VALIDATION);
    s_physicalDevice = std::make_unique<PhysicalDevice>(*s_instance);
    s_device = std::make_shared<Device>(s_instance, *s_physicalDevice, ENABLE_VALIDATION);
    s_cmdQueue = std::make_unique<CmdQueue>(s_device, *s_physicalDevice, QueueType::GRAPHICS);
    s_descriptorPool = std::make_shared<DescriptorPool>(s_device);
    s_sampler = std::make_shared<Sampler>("Sampler", s_device, *s_physicalDevice,
        SamplerInfo{});

    BuildSwapchain();
    BuildRenderTargets();
    BuildDescriptors();
    BuildPipelines();

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
    s_cmdQueue->ProcessCmdLists();

    if (Window::IsActive())
    {
        if (Window::WasResized())
        {
            BuildSwapchain();
            BuildRenderTargets();
            BuildPipelines();
        }

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

        Rect2D swapchainExtent = s_swapchain->Extent();
        size_t currentFrame = static_cast<size_t>(s_swapchain->GetCurrentFrameIdx());

        // Swapchain present pass
        s_cmdList->BeginRenderPass(s_renderPass, *s_framebuffer,
            Color(0.6f, 0.8f, 1.0f, 1.0f));
        Rect2D imgExtent(static_cast<float>(s_colorImage->Width()),
            static_cast<float>(s_colorImage->Height()));
        s_cmdList->SetScissor(imgExtent);
        s_cmdList->SetViewport(imgExtent);
        // TODO: render da shit
        s_cmdList->EndRenderPass();

        s_cmdList->TransitionImageLayout(s_colorImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        s_cmdList->BeginRenderPass(s_swapchain->GetRenderPass(),
            s_swapchain->GetCurrentFramebuffer(),
            Color(0.1f, 0.1f, 0.1f, 1.0f));
        s_cmdList->SetScissor(swapchainExtent);
        s_cmdList->SetViewport(swapchainExtent);
        s_cmdList->BindGraphicsPipeline(s_presentPipeline);
        s_presentDescriptorSets[currentFrame]->SetImage(
            0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, s_colorImage, s_sampler);
        s_cmdList->BindDescriptorSet(s_presentDescriptorSets[currentFrame], 0);
        s_cmdList->Draw(3);
        // TODO: draw imgui
        s_cmdList->EndRenderPass();
        s_cmdList->TransitionImageLayout(
            s_colorImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

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