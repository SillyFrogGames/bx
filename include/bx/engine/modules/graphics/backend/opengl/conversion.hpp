#pragma once

#include "bx/engine/core/guard.hpp"
#include "bx/engine/containers/string.hpp"

#include "opengl_api.hpp"
#include "bx/engine/modules/graphics/type.hpp"

namespace Gl
{
	GLenum TextureDimensionToGl(const TextureDimension& dimension);
	GLenum TextureFormatToGlType(const TextureFormat& format);
	GLenum TextureFormatToGlFormat(const TextureFormat& format);
}