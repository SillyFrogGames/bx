#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>
#include <bx/engine/modules/graphics.hpp>

class Renderer : public System
{
public:
	/*Vec4i GetLightIndices(const Vec3& pos);

	void UpdateAnimators();
	void UpdateCameras();
	void UpdateLights();

	void CollectDrawCommands();

	void DrawCommand(const GraphicsHandle pipeline, u32 numResourceBindings, const GraphicsHandle* pResourcesBindings, u32 numBuffers, const GraphicsHandle* pBuffers, const u64* offset, const GraphicsHandle indexBuffer, u32 count);
	void DrawCommands();

	void BindConstants(const Mat4& viewMtx, const Mat4& projMtx, const Mat4& viewProjMtx);*/

private:
	void Initialize() override;
	void Shutdown() override;

	void Update() override;
	void Render() override;
};