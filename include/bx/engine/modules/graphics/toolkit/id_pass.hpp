#pragma once

#include "bx/engine/core/guard.hpp"

#include "bx/engine/modules/graphics.hpp"

class IdPass : NoCopy
{
public:
	IdPass(TextureHandle colorTarget, TextureHandle depthTarget);
	~IdPass();

	void Dispatch();

	static void ClearPipelineCache();

private:
	BindGroupHandle bindGroup;
	TextureHandle colorTarget;
	TextureHandle depthTarget;
	TextureViewHandle colorTargetView;
	TextureViewHandle depthTargetView;
	u32 width, height;
};