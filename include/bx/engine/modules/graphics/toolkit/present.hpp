#pragma once

#include "bx/engine/core/guard.hpp"

#include "bx/engine/modules/graphics.hpp"

class PresentPass : NoCopy
{
public:
	PresentPass(TextureHandle hdrTexture);
	~PresentPass();

	void Dispatch();

	static void ClearPipelineCache();

private:
	BindGroupHandle bindGroup;
	TextureViewHandle hdrTextureView;
	u32 width, height;
};