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

#ifdef BX_WINDOW_GLFW_BACKEND
#include "bx/engine/modules/window/backend/window_glfw.hpp"
#endif


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

    std::shared_ptr<Vk::Instance> instance = std::make_shared<Vk::Instance>((void*)glfwWindow, true);
    std::unique_ptr<Vk::PhysicalDevice> physicalDevice = std::make_unique<Vk::PhysicalDevice>(*instance);
    std::shared_ptr<Vk::Device> device = std::make_shared<Vk::Device>(instance, *physicalDevice, true);

    std::shared_ptr<Vk::Fence> fence = std::make_shared<Vk::Fence>("my fence", device);
    std::shared_ptr<Vk::Semaphore> semaphore = std::make_shared<Vk::Semaphore>("my semaphore", device);

    std::shared_ptr<Vk::Image> image = std::make_shared<Vk::Image>("my image", device, *physicalDevice, 512, 512, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_FORMAT_R8G8B8A8_SRGB);
    std::shared_ptr<Vk::Image> depthImage = std::make_shared<Vk::Image>("my depth image", device, *physicalDevice, 512, 512, 1, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D24_UNORM_S8_UINT);
    std::shared_ptr<Vk::RenderPass> renderPass = std::make_shared<Vk::RenderPass>("my render pass", device, List<VkFormat>{VK_FORMAT_R8G8B8A8_SRGB}, Optional<VkFormat>::Some(VK_FORMAT_D24_UNORM_S8_UINT));
    std::shared_ptr<Vk::Framebuffer> framebuffer = std::make_shared<Vk::Framebuffer>("my framebuffer", device, List<std::shared_ptr<Vk::Image>>{image, depthImage}, renderPass);

    return true;
}

void Graphics::Shutdown()
{
    
}

void Graphics::Reload()
{
    // TODO: No implementation
}

void Graphics::NewFrame()
{
    
}

void Graphics::EndFrame()
{
    
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