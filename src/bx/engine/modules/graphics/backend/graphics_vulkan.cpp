#include "bx/engine/modules/graphics/backend/graphics_vulkan.hpp"

#include "bx/engine/core/file.hpp"
#include "bx/engine/core/profiler.hpp"
#include "bx/engine/core/memory.hpp"
#include "bx/engine/core/macros.hpp"

#include "bx/engine/modules/window.hpp"

#include "bx/engine/modules/graphics/backend/vulkan/instance.hpp"
#include "bx/engine/modules/graphics/backend/vulkan/physical_device.hpp"

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

    std::unique_ptr<Vk::Instance> instance = std::make_unique<Vk::Instance>((void*)glfwWindow, true);
    std::unique_ptr<Vk::PhysicalDevice> physicalDevice = std::make_unique<Vk::PhysicalDevice>(*instance);

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