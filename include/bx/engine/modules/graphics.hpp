#pragma once

#include "bx/engine/modules/graphics/type.hpp"

class Graphics
{
public:
	static TextureFormat GetColorBufferFormat();
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
	static void DrawIndexed(const DrawIndexedAttribs& attribs);

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