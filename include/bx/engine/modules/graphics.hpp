#pragma once

#include "bx/engine/modules/graphics/type.hpp"

class Graphics
{
public:
	/*static TextureFormat GetColorBufferFormat();
	static TextureFormat GetDepthBufferFormat();

	static GraphicsHandle GetCurrentBackBufferRT();
	static GraphicsHandle GetDepthBuffer();

	static void SetRenderTarget(const GraphicsHandle renderTarget, const GraphicsHandle depthStencil);
	static void ReadPixels(u32 x, u32 y, u32 w, u32 h, void* pixelData, const GraphicsHandle renderTarget);

	static void SetViewport(const f32 viewport[4]);

	static void ClearRenderTarget(const GraphicsHandle renderTarget, const f32 clearColor[4]);
	static void ClearDepthStencil(const GraphicsHandle depthStencil, GraphicsClearFlags flags, f32 depth, i32 stencil);

	static GraphicsHandle CreateShader(const ShaderInfo& info);
	static void DestroyShader(const GraphicsHandle shader);

	static GraphicsHandle CreateTexture(const TextureInfo& info);
	static GraphicsHandle CreateTexture(const TextureInfo& info, const BufferData& data);
	static void DestroyTexture(const GraphicsHandle texture);

	static GraphicsHandle CreateResourceBinding(const ResourceBindingInfo& info);
	static void DestroyResourceBinding(const GraphicsHandle resources);
	static void BindResource(const GraphicsHandle resources, const char* name, GraphicsHandle resource);

	static GraphicsHandle CreatePipeline(const PipelineInfo& info);
	static void DestroyPipeline(const GraphicsHandle pipeline);
	static void SetPipeline(const GraphicsHandle pipeline);
	static void CommitResources(const GraphicsHandle pipeline, const GraphicsHandle resources);

	static GraphicsHandle CreateBuffer(const BufferInfo& info);
	static GraphicsHandle CreateBuffer(const BufferInfo& info, const BufferData& data);
	static void DestroyBuffer(const GraphicsHandle buffer);
	static void UpdateBuffer(const GraphicsHandle buffer, const BufferData& data);

	static void SetVertexBuffers(i32 i, i32 count, const GraphicsHandle* pBuffers, const u64* offset);
	static void SetIndexBuffer(const GraphicsHandle buffer, i32 i);

	static void Draw(const DrawAttribs& attribs);
	static void DrawIndexed(const DrawIndexedAttribs& attribs);*/

	// Helper resources
	static const HBuffer EmptyBuffer();
	static const HTexture EmptyTexture();

	// Swapchain
	static TextureFormat GetSwapchainFormat();

	// Resource creation and destruction
	static const TextureCreateInfo& GetTextureCreateInfo(HTexture texture);
	static HTexture CreateTexture(const TextureCreateInfo& createInfo);
	template <typename T>
	static HTexture CreateTextureWithData(const TextureCreateInfo& createInfo, const T& data)
	{
		CreateTextureWithDataPtr(createInfo, static_cast<const void*>(&data));
	}
	static HTexture CreateTextureWithDataPtr(const TextureCreateInfo& createInfo, const void* data);
	static void DestroyTexture(HTexture& texture);

	static HTextureView CreateTextureView(HTexture texture);
	static void DestroyTextureView(HTextureView& textureView);

	static const SamplerCreateInfo& GetSamplerCreateInfo(HSampler sampler);
	static HSampler CreateSampler(const SamplerCreateInfo& create);
	static void DestroySampler(HSampler& sampler);

	static const BufferCreateInfo& GetBufferCreateInfo(HBuffer buffer);
	static HBuffer CreateBuffer(const BufferCreateInfo& createInfo);
	template <typename T>
	static HBuffer CreateBufferWithData(const BufferCreateInfo& createInfo, const T& data)
	{
		CreateBufferWithDataPtr(createInfo, static_cast<const void*>(&data));
	}
	static HBuffer CreateBufferWithDataPtr(const BufferCreateInfo& createInfo, const void* data);
	static void DestroyBuffer(HBuffer& buffer);

	static const ShaderCreateInfo& GetShaderCreateInfo(HShader shader);
	static HShader CreateShader(const ShaderCreateInfo& createInfo);
	static void DestroyShader(HShader& shader);
	
	static const GraphicsPipelineCreateInfo& GetGraphicsPipelineCreateInfo(HGraphicsPipeline graphicsPipeline);
	static HGraphicsPipeline CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
	static void DestroyGraphicsPipeline(HGraphicsPipeline& graphicsPipeline);
	static const ComputePipelineCreateInfo& GetComputePipelineCreateInfo(HComputePipeline computePipeline);
	static HComputePipeline CreateComputePipeline(const ComputePipelineCreateInfo& createInfo);
	static void DestroyComputePipeline(HComputePipeline& graphicsPipeline);

	static HBindGroupLayout GetBindGroupLayout(HGraphicsPipeline graphicsPipeline, u32 bindGroup);
	static HBindGroupLayout GetBindGroupLayout(HComputePipeline computePipeline, u32 bindGroup);
	static HBindGroup CreateBindGroup(const BindGroupCreateInfo& createInfo);
	static void DestroyBindGroup(HBindGroup& bindGroup);

	// Cmds
	static HRenderPass BeginRenderPass(const RenderPassDescriptor& descriptor);
	static void SetGraphicsPipeline(HRenderPass renderPass, HGraphicsPipeline graphicsPipeline);
	static void SetVertexBuffer(HRenderPass renderPass, u32 slot, const BufferSlice& bufferSlice);
	static void SetIndexBuffer(HRenderPass renderPass, const BufferSlice& bufferSlice, IndexFormat format);
	static void SetBindGroup(HRenderPass renderPass, u32 index, HBindGroup bindGroup);
	static void Draw(HRenderPass renderPass, u32 vertexCount, u32 firstVertex = 0, u32 instanceCount = 1, u32 firstInstance = 0);
	static void DrawIndexed(HRenderPass renderPass, u32 indexCount, u32 firstIndex = 0, u32 baseVertex = 0, u32 instanceCount = 1, u32 firstInstance = 0);
	static void EndRenderPass(HRenderPass& renderPass);

	// Write data to buffer, write is queued untill `FlushBufferWrites` is called, a compute or render pass is started or when the frame is finished.
	// Data is copied over immediately and can be freed after calling
	template <typename T>
	static void WriteBuffer(HBuffer buffer, u64 offset, const T& data)
	{
		WriteBufferPtr(buffer, offset, static_cast<const void*>(&data));
	}
	// Write data to buffer, write is queued untill `FlushBufferWrites` is called, a compute or render pass is started or when the frame is finished.
	// Data is copied over immediately and can be freed after calling
	static void WriteBufferPtr(HBuffer buffer, u64 offset, const void* data);
	static void FlushBufferWrites();
	// Write data to texture, write is queued untill `FlushTextureWrites` is called, a compute or render pass is started or when the frame is finished.
	// Data is copied over immediately and can be freed after calling
	static void WriteTexturePtr(HTexture texture, const u8* data, const ImageDataLayout& dataLayout, const Extend3D& size);
	static void FlushTextureWrites();

	// Debug draw utilities
	static void DebugLine(const Vec3& a, const Vec3& b, u32 color = 0xFFFFFFFF, f32 lifespan = 0.0f);

	static void UpdateDebugLines();
	static void DrawDebugLines(const Mat4& viewProj);
	static void ClearDebugLines();

	static void DebugDraw(const Mat4& viewProj, const DebugDrawAttribs& attribs, const List<DebugVertex>& vertices);

private:
	friend class Runtime;
	friend class Module;

	static bool Initialize();
	static void Reload();
	static void Shutdown();

	static void NewFrame();
	static void EndFrame();
};