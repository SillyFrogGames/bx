#include "bx/engine/modules/graphics/backend/opengl/conversion.hpp"

#include "bx/engine/core/macros.hpp"

namespace Gl
{
	GLenum TextureDimensionToGl(const TextureDimension& dimension, u32 depthOrArrayLayers)
	{
		switch (dimension)
		{
		case TextureDimension::D1:
			return (depthOrArrayLayers == 1) ? GL_TEXTURE_1D : GL_TEXTURE_1D_ARRAY;
		case TextureDimension::D2:
			return (depthOrArrayLayers == 1) ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
		case TextureDimension::D3:
			return GL_TEXTURE_3D;
		default:
			BX_FAIL("{} not supported.", dimension);
		}
	}

	GLenum TextureFormatToGlType(const TextureFormat& format)
	{
		switch (format)
		{
		case TextureFormat::R8_UNORM:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::R8_SNORM:
			return GL_BYTE;
		case TextureFormat::R8_UINT:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::R8_SINT:
			return GL_BYTE;

		case TextureFormat::R16_UINT:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::R16_SINT:
			return GL_SHORT;
		case TextureFormat::R16_UNORM:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::R16_SNORM:
			return GL_SHORT;
		case TextureFormat::R16_FLOAT:
			return GL_HALF_FLOAT;
		case TextureFormat::RG8_UNORM:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RG8_SNORM:
			return GL_BYTE;
		case TextureFormat::RG8_UINT:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RG8_SINT:
			return GL_BYTE;

		case TextureFormat::R32_UINT:
			return GL_UNSIGNED_INT;
		case TextureFormat::R32_SINT:
			return GL_INT;
		case TextureFormat::R32_FLOAT:
			return GL_FLOAT;
		case TextureFormat::RG16_UINT:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::RG16_SINT:
			return GL_SHORT;
		case TextureFormat::RG16_UNORM:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::RG16_SNORM:
			return GL_SHORT;
		case TextureFormat::RG16_FLOAT:
			return GL_HALF_FLOAT;
		case TextureFormat::RGBA8_UNORM:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGBA8_UNORM_SRGB:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGBA8_SNORM:
			return GL_BYTE;
		case TextureFormat::RGBA8_UINT:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGBA8_SINT:
			return GL_BYTE;
		case TextureFormat::BGRA8_UNORM:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::BGRA8_UNORM_SRGB:
			return GL_UNSIGNED_BYTE;

		case TextureFormat::RGB9E5_UFLOAT:
			GL_UNSIGNED_INT_5_9_9_9_REV;
		case TextureFormat::RGB10A2_UINT:
			return GL_UNSIGNED_INT_10_10_10_2;
		case TextureFormat::RGB10A2_UNORM:
			return GL_UNSIGNED_INT_10_10_10_2;
		case TextureFormat::RG11B10_FLOAT:
			return GL_FLOAT;

		case TextureFormat::RG32_UINT:
			return GL_UNSIGNED_INT;
		case TextureFormat::RG32_SINT:
			return GL_INT;
		case TextureFormat::RG32_FLOAT:
			return GL_FLOAT;
		case TextureFormat::RGBA16_UINT:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::RGBA16_SINT:
			return GL_SHORT;
		case TextureFormat::RGBA16_UNORM:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::RGBA16_SNORM:
			return GL_SHORT;
		case TextureFormat::RGBA16_FLOAT:
			return GL_HALF_FLOAT;

		case TextureFormat::RGBA32_UINT:
			return GL_UNSIGNED_INT;
		case TextureFormat::RGBA32_SINT:
			return GL_INT;
		case TextureFormat::RGBA32_FLOAT:
			return GL_FLOAT;

		// TODO: figure out how to support these
		case TextureFormat::STENCIL8:
			return GL_FLOAT;
		case TextureFormat::DEPTH16_UNORM:
			return GL_FLOAT;
		case TextureFormat::DEPTH24_PLUS:
			return GL_FLOAT;
		case TextureFormat::DEPTH24_PLUS_STENCIL8:
			return GL_FLOAT;
		case TextureFormat::DEPTH32_FLOAT:
			return GL_FLOAT;
		case TextureFormat::DEPTH32_FLOAT_STENCIL8:
			return GL_FLOAT;

		/*case TextureFormat::BC1_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC1_RGBA_UNORM_SRGB:
			return GL_FLOAT;
		case TextureFormat::BC2_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC2_RGBA_UNORM_SRGB:
			return GL_FLOAT;
		case TextureFormat::BC3_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC3_RGBA_UNORM_SRGB:
			return GL_FLOAT;
		case TextureFormat::BC4_R_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC4_R_SNORM:
			return GL_FLOAT;
		case TextureFormat::BC5_RG_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC5_RG_SNORM:
			return GL_FLOAT;
		case TextureFormat::BC6H_RGB_UFLOAT:
			return GL_FLOAT;
		case TextureFormat::BC6H_RGB_FLOAT:
			return GL_FLOAT;
		case TextureFormat::BC7_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC7_RGBA_UNORM_SRGB:
			return GL_FLOAT;*/

		default:
			BX_FAIL("{} not supported.", format);
		}
	}

	GLenum TextureFormatToGlFormat(const TextureFormat& format)
	{
		switch (format)
		{
		case TextureFormat::R8_UNORM:
			return GL_RED;
		case TextureFormat::R8_SNORM:
			return GL_RED;
		case TextureFormat::R8_UINT:
			return GL_RED;
		case TextureFormat::R8_SINT:
			return GL_RED;

		case TextureFormat::R16_UINT:
			return GL_RED;
		case TextureFormat::R16_SINT:
			return GL_RED;
		case TextureFormat::R16_UNORM:
			return GL_RED;
		case TextureFormat::R16_SNORM:
			return GL_RED;
		case TextureFormat::R16_FLOAT:
			return GL_RED;
		case TextureFormat::RG8_UNORM:
			return GL_RG;
		case TextureFormat::RG8_SNORM:
			return GL_RG;
		case TextureFormat::RG8_UINT:
			return GL_RG;
		case TextureFormat::RG8_SINT:
			return GL_RG;

		case TextureFormat::R32_UINT:
			return GL_RED;
		case TextureFormat::R32_SINT:
			return GL_RED;
		case TextureFormat::R32_FLOAT:
			return GL_RED;
		case TextureFormat::RG16_UINT:
			return GL_RG;
		case TextureFormat::RG16_SINT:
			return GL_RG;
		case TextureFormat::RG16_UNORM:
			return GL_RG;
		case TextureFormat::RG16_SNORM:
			return GL_RG;
		case TextureFormat::RG16_FLOAT:
			return GL_RG;
		case TextureFormat::RGBA8_UNORM:
			return GL_RGBA;
		case TextureFormat::RGBA8_UNORM_SRGB:
			return GL_RGBA;
		case TextureFormat::RGBA8_SNORM:
			return GL_RGBA;
		case TextureFormat::RGBA8_UINT:
			return GL_RGBA;
		case TextureFormat::RGBA8_SINT:
			return GL_RGBA;
		/*case TextureFormat::BGRA8_UNORM:
			return GL_BGRA;*/
		/*case TextureFormat::BGRA8_UNORM_SRGB:
			return GL_UNSIGNED_BYTE;*/

		case TextureFormat::RGB9E5_UFLOAT:
			return GL_RGB;
		case TextureFormat::RGB10A2_UINT:
			return GL_RGBA;
		case TextureFormat::RGB10A2_UNORM:
			return GL_RGBA;
		case TextureFormat::RG11B10_FLOAT:
			return GL_RGB;

		case TextureFormat::RG32_UINT:
			return GL_RG;
		case TextureFormat::RG32_SINT:
			return GL_RG;
		case TextureFormat::RG32_FLOAT:
			return GL_RG;
		case TextureFormat::RGBA16_UINT:
			return GL_RGBA;
		case TextureFormat::RGBA16_SINT:
			return GL_RGBA;
		case TextureFormat::RGBA16_UNORM:
			return GL_RGBA;
		case TextureFormat::RGBA16_SNORM:
			return GL_RGBA;
		case TextureFormat::RGBA16_FLOAT:
			return GL_RGBA;

		case TextureFormat::RGBA32_UINT:
			return GL_RGBA;
		case TextureFormat::RGBA32_SINT:
			return GL_RGBA;
		case TextureFormat::RGBA32_FLOAT:
			return GL_RGBA;

			// TODO: figure out how to support these
			/*case TextureFormat::STENCIL8:
				return GL_BYTE;*/
		case TextureFormat::DEPTH16_UNORM:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH24_PLUS:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH24_PLUS_STENCIL8:
			return GL_DEPTH_STENCIL;
		case TextureFormat::DEPTH32_FLOAT:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH32_FLOAT_STENCIL8:
			return GL_DEPTH_STENCIL;

			/*case TextureFormat::BC1_RGBA_UNORM:
				return GL_FLOAT;
			case TextureFormat::BC1_RGBA_UNORM_SRGB:
				return GL_FLOAT;
			case TextureFormat::BC2_RGBA_UNORM:
				return GL_FLOAT;
			case TextureFormat::BC2_RGBA_UNORM_SRGB:
				return GL_FLOAT;
			case TextureFormat::BC3_RGBA_UNORM:
				return GL_FLOAT;
			case TextureFormat::BC3_RGBA_UNORM_SRGB:
				return GL_FLOAT;
			case TextureFormat::BC4_R_UNORM:
				return GL_FLOAT;
			case TextureFormat::BC4_R_SNORM:
				return GL_FLOAT;
			case TextureFormat::BC5_RG_UNORM:
				return GL_FLOAT;
			case TextureFormat::BC5_RG_SNORM:
				return GL_FLOAT;
			case TextureFormat::BC6H_RGB_UFLOAT:
				return GL_FLOAT;
			case TextureFormat::BC6H_RGB_FLOAT:
				return GL_FLOAT;
			case TextureFormat::BC7_RGBA_UNORM:
				return GL_FLOAT;
			case TextureFormat::BC7_RGBA_UNORM_SRGB:
				return GL_FLOAT;*/

		default:
			BX_FAIL("{} not supported.", format);
		}
	}

	GLenum TextureFormatToGlInternalFormat(const TextureFormat& format)
	{
		switch (format)
		{
		case TextureFormat::R8_UNORM:
			return GL_R8;
		case TextureFormat::R8_SNORM:
			return GL_R8_SNORM;
		case TextureFormat::R8_UINT:
			return GL_R8UI;
		case TextureFormat::R8_SINT:
			return GL_R8I;

		case TextureFormat::R16_UINT:
			return GL_R16UI;
		case TextureFormat::R16_SINT:
			return GL_R16I;
		case TextureFormat::R16_UNORM:
			return GL_R16;
		case TextureFormat::R16_SNORM:
			return GL_R16_SNORM;
		case TextureFormat::R16_FLOAT:
			return GL_R16F;
		case TextureFormat::RG8_UNORM:
			return GL_RG8;
		case TextureFormat::RG8_SNORM:
			return GL_RG8_SNORM;
		case TextureFormat::RG8_UINT:
			return GL_RG8UI;
		case TextureFormat::RG8_SINT:
			return GL_RG8I;

		case TextureFormat::R32_UINT:
			return GL_R32UI;
		case TextureFormat::R32_SINT:
			return GL_R32I;
		case TextureFormat::R32_FLOAT:
			return GL_R32F;
		case TextureFormat::RG16_UINT:
			return GL_RG16UI;
		case TextureFormat::RG16_SINT:
			return GL_RG16I;
		case TextureFormat::RG16_UNORM:
			return GL_RG16;
		case TextureFormat::RG16_SNORM:
			return GL_RG16_SNORM;
		case TextureFormat::RG16_FLOAT:
			return GL_RG16F;
		case TextureFormat::RGBA8_UNORM:
			return GL_RGBA8;
		case TextureFormat::RGBA8_UNORM_SRGB:
			return GL_SRGB8_ALPHA8;
		case TextureFormat::RGBA8_SNORM:
			return GL_RGBA8_SNORM;
		case TextureFormat::RGBA8_UINT:
			return GL_RGBA8UI;
		case TextureFormat::RGBA8_SINT:
			return GL_RGBA8I;
		/*case TextureFormat::BGRA8_UNORM:
			return GL_BGRA;*/
		/*case TextureFormat::BGRA8_UNORM_SRGB:
			return GL_UNSIGNED_BYTE;*/

		case TextureFormat::RGB9E5_UFLOAT:
			GL_RGB9_E5;
		case TextureFormat::RGB10A2_UINT:
			return GL_RGB10_A2UI;
		case TextureFormat::RGB10A2_UNORM:
			return GL_RGB10_A2;
		case TextureFormat::RG11B10_FLOAT:
			return GL_R11F_G11F_B10F;

		case TextureFormat::RG32_UINT:
			return GL_RG32UI;
		case TextureFormat::RG32_SINT:
			return GL_RG32I;
		case TextureFormat::RG32_FLOAT:
			return GL_RG32F;
		case TextureFormat::RGBA16_UINT:
			return GL_RGBA16UI;
		case TextureFormat::RGBA16_SINT:
			return GL_RGBA16I;
		case TextureFormat::RGBA16_UNORM:
			return GL_RGBA16;
		case TextureFormat::RGBA16_SNORM:
			return GL_RGBA16_SNORM;
		case TextureFormat::RGBA16_FLOAT:
			return GL_RGBA16F;

		case TextureFormat::RGBA32_UINT:
			return GL_RGBA32UI;
		case TextureFormat::RGBA32_SINT:
			return GL_RGBA32I;
		case TextureFormat::RGBA32_FLOAT:
			return GL_RGBA32F;

		// TODO: figure out how to support these
		/*case TextureFormat::STENCIL8:
			return GL_BYTE;*/
		case TextureFormat::DEPTH16_UNORM:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH24_PLUS:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH24_PLUS_STENCIL8:
			return GL_DEPTH_STENCIL;
		case TextureFormat::DEPTH32_FLOAT:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH32_FLOAT_STENCIL8:
			return GL_DEPTH_STENCIL;

		/*case TextureFormat::BC1_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC1_RGBA_UNORM_SRGB:
			return GL_FLOAT;
		case TextureFormat::BC2_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC2_RGBA_UNORM_SRGB:
			return GL_FLOAT;
		case TextureFormat::BC3_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC3_RGBA_UNORM_SRGB:
			return GL_FLOAT;
		case TextureFormat::BC4_R_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC4_R_SNORM:
			return GL_FLOAT;
		case TextureFormat::BC5_RG_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC5_RG_SNORM:
			return GL_FLOAT;
		case TextureFormat::BC6H_RGB_UFLOAT:
			return GL_FLOAT;
		case TextureFormat::BC6H_RGB_FLOAT:
			return GL_FLOAT;
		case TextureFormat::BC7_RGBA_UNORM:
			return GL_FLOAT;
		case TextureFormat::BC7_RGBA_UNORM_SRGB:
			return GL_FLOAT;*/

		default:
			BX_FAIL("{} not supported.", format);
		}
	}
}