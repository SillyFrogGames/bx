#pragma once

#include "bx/engine/core/math.hpp"
#include "bx/engine/core/type.hpp"
#include "bx/engine/core/macros.hpp"
#include "bx/engine/containers/optional.hpp"

struct HBuffer
{
	u64 id;
	static const HBuffer null;
};

struct HTexture
{
	u64 id;
	static const HTexture null;
};

struct HShader
{
	u64 id;
	static const HShader null;
};

struct HPipeline
{
	u64 id;
	static const HPipeline null;
};

ENUM(GraphicsClearFlags,
	COLOR = BX_BIT(0),
	DEPTH = BX_BIT(1),
	STENCIL = BX_BIT(2)
);

ENUM(ShaderType,
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	COMPUTE
);

ENUM(BufferUsageFlags,
	MAP_READ = BX_BIT(0),
	MAP_WRITE = BX_BIT(1),
	COPY_SRC = BX_BIT(2),
	COPY_DST = BX_BIT(3),
	INDEX = BX_BIT(4),
	VERTEX = BX_BIT(5),
	UNIFORM = BX_BIT(6),
	STORAGE = BX_BIT(7),
	INDIRECT = BX_BIT(8)
);
ENUM(BufferAccess,
	READ,
	WRITE,
	READ_WRITE
);

ENUM(TextureFormat,
	R8_UNORM,
	R8_SNORM,
	R8_UINT,
	R8_SINT,

	R16_UINT,
	R16_SINT,
	R16_UNORM,
	R16_SNORM,
	R16_FLOAT,
	RG8_UNORM,
	RG8_SNORM,
	RG8_UINT,
	RG8_SINT,

	R32_UINT,
	R32_SINT,
	R32_FLOAT,
	RG16_UINT,
	RG16_SINT,
	RG16_UNORM,
	RG16_SNORM,
	RG16_FLOAT,
	RGBA8_UNORM,
	RGBA8_UNORM_SRGB,
	RGBA8_SNORM,
	RGBA8_UINT,
	RGBA8_SINT,
	BGRA8_UNORM,
	BGRA8_UNORM_SRGB,

	RGB9E5_UFLOAT,
	RGB10A2_UINT,
	RGB10A2_UNORM,
	RG11B10_FLOAT,

	RG32_UINT,
	RG32_SINT,
	RG32_FLOAT,
	RGBA16_UINT,
	RGBA16_SINT,
	RGBA16_UNORM,
	RGBA16_SNORM,
	RGBA16_FLOAT,

	RGBA32_UINT,
	RGBA32_SINT,
	RGBA32_FLOAT,

	STENCIL8,
	DEPTH16_UNORM,
	DEPTH24_PLUS,
	DEPTH24_PLUS_STENCIL8,
	DEPTH32_FLOAT,
	DEPTH32_FLOAT_STENCIL8,

	BC1_RGBA_UNORM,
	BC1_RGBA_UNORM_SRGB,
	BC2_RGBA_UNORM,
	BC2_RGBA_UNORM_SRGB,
	BC3_RGBA_UNORM,
	BC3_RGBA_UNORM_SRGB,
	BC4_R_UNORM,
	BC4_R_SNORM,
	BC5_RG_UNORM,
	BC5_RG_SNORM,
	BC6H_RGB_UFLOAT,
	BC6H_RGB_FLOAT,
	BC7_RGBA_UNORM,
	BC7_RGBA_UNORM_SRGB
);
ENUM(TextureUsageFlags,
	COPY_SRC = BX_BIT(0),
	COPY_DST = BX_BIT(1),
	TEXTURE_BINDING = BX_BIT(2),
	STORAGE_BINDING = BX_BIT(3),
	RENDER_ATTACHMENT = BX_BIT(4)
);

ENUM(ResourceBindingType,
	BUFFER,
	SAMPLER,
	TEXTURE,
	STORAGE_TEXTURE,
	ACCELERATION_STRUCTURE
);

ENUM(GraphicsPipelineTopology,
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
);
ENUM(GraphicsPipelineFaceCull,
	CW,
	CCW
);

ENUM(VertexFormat,
	UINT_8X2,
	UINT_8X4,
	SINT_8X2,
	SINT_8X4,
	UNORM_8X2,
	UNORM_8X4,
	SNORM_8X2,
	SNORM_8X4,
	UINT_16X2,
	UINT_16X4,
);

struct VertexAttribute
{
	VertexFormat format;
	u32 offset;
	u32 location;
};

struct VertexBufferLayout
{
	u32 stride;
	List<VertexAttribute> attributes;
};

struct GraphicsPipelineCreateInfo
{
	HShader vertexShader = HShader::null;
	HShader fragmentShader = HShader::null;
	// TODO
	// HShader geometryShader = HShader::null;
	// HShader tessalationShader = HShader::null;

	// vertex layout
	// color targets
	GraphicsPipelineTopology topology = GraphicsPipelineTopology::TRIANGLE_LIST;
	Optional<GraphicsPipelineFaceCull> faceCull = Optional<GraphicsPipelineFaceCull>::None();
	// pipeline layout
	Optional<TextureFormat> depthFormat = Optional<TextureFormat>::None();
};

struct ShaderCreateInfo
{
	ShaderType shaderType = ShaderType::VERTEX;
	String src;
};

struct BufferCreateInfo
{
	BufferUsageFlags usageFlags = 0;
	BufferAccess access = BufferAccess::READ_WRITE;
	u32 strideBytes = 0;
};

struct TextureCreateInfo
{
	TextureFormat format = TextureFormat::RGBA8_UNORM_SRGB;
	TextureUsageFlags usageFlags = 0;
	u32 width = 0;
	u32 height = 0;
	// TODO
	// u32 depth = 1;
	// u32 mips = 1;
};

struct BufferData
{
	BufferData() {}
	BufferData(const void* pData, u32 dataSize)
		: pData(pData)
		, dataSize(dataSize) {}

	const void* pData = nullptr;
	u32 dataSize = 0;
};

struct ResourceBindingElement
{
	ResourceBindingElement() {}
	ResourceBindingElement(ShaderType shaderType, const char* name, u32 count, ResourceBindingType type, ResourceBindingAccess access)
		: shaderType(shaderType)
		, name(name)
		, count(count)
		, type(type)
		, access(access) {}

	ShaderType shaderType = ShaderType::UNKNOWN;
	const char* name = nullptr;
	u32 count = 0;
	ResourceBindingType type = ResourceBindingType::UNKNOWN;
	ResourceBindingAccess access = ResourceBindingAccess::STATIC;
};

struct ResourceBindingInfo
{
	const ResourceBindingElement* resources = nullptr;
	u32 numResources = 0;
};

struct LayoutElement
{
	LayoutElement() {}
	LayoutElement(u32 inputIndex, u32 bufferSlot, u32 numComponents, GraphicsValueType valueType, bool isNormalized, u32 relativeOffset, u32 instanceDataStepRate)
		: inputIndex(inputIndex)
		, bufferSlot(bufferSlot)
		, numComponents(numComponents)
		, valueType(valueType)
		, isNormalized(isNormalized)
		, relativeOffset(relativeOffset)
		, instanceDataStepRate(instanceDataStepRate) {}

	u32 inputIndex = 0;
	u32 bufferSlot = 0;
	u32 numComponents = 0;
	GraphicsValueType valueType = GraphicsValueType::FLOAT32;
	bool isNormalized = false;
	u32 relativeOffset = 0;
	u32 instanceDataStepRate = 0;
};

struct PipelineInfo
{
	u32 numRenderTargets = 0;
	TextureFormat renderTargetFormats[8] = { TextureFormat::UNKNOWN };
	TextureFormat depthStencilFormat = TextureFormat::UNKNOWN;

	PipelineTopology topology = PipelineTopology::UNDEFINED;
	PipelineFaceCull faceCull = PipelineFaceCull::NONE;

	bool depthEnable = true;
	bool blendEnable = false;

	GraphicsHandle vertShader = INVALID_GRAPHICS_HANDLE;
	GraphicsHandle pixelShader = INVALID_GRAPHICS_HANDLE;

	const LayoutElement* layoutElements = nullptr;
	u32 numElements = 0;
};

struct DrawAttribs
{
	DrawAttribs() {}
	DrawAttribs(u32 numVertices)
		: numVertices(numVertices) {}

	u32 numVertices = 0;
};

struct DrawIndexedAttribs
{
	DrawIndexedAttribs() {}
	DrawIndexedAttribs(GraphicsValueType indexType, u32 numIndices)
		: indexType(indexType), numIndices(numIndices) {}

	GraphicsValueType indexType = GraphicsValueType::UINT32;
	u32 numIndices = 0;
};

struct DebugVertex
{
	DebugVertex() {}
	DebugVertex(const Vec3& vert, u32 col)
		: vert(vert), col(col) {}

	Vec3 vert{ 0, 0, 0 };
	u32 col{ 0 };
};

struct DebugDrawAttribs
{
};