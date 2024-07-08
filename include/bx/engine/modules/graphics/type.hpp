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

struct HSampler
{
	u64 id;
	static const HSampler null;
};

struct HTexture
{
	u64 id;
	static const HTexture null;
};

struct HTextureView
{
	u64 id;
	static const HTextureView null;
};

struct HShader
{
	u64 id;
	static const HShader null;
};

struct HGraphicsPipeline
{
	u64 id;
	static const HGraphicsPipeline null;
};

struct HComputePipeline
{
	u64 id;
	static const HComputePipeline null;
};

struct HRenderPass
{
	u64 id;
	static const HRenderPass null;
};

struct HBindGroupLayout
{
	u64 id;
	static const HBindGroupLayout null;
};

struct HBindGroup
{
	u64 id;
	static const HBindGroup null;
};

ENUM(ShaderType,
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	COMPUTE
);

ENUM(ShaderStageFlags,
	VERTEX = BX_BIT(0),
	FRAGMENT = BX_BIT(1),
	COMPUTE = BX_BIT(2)
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

ENUM(StorageTextureAccess,
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

ENUM(SamplerBorderColor,
	TRANSPARENT_BLACK,
	OPAQUE_BLACK,
	OPAQUE_WHITE
);

ENUM(AddressMode,
	CLAMP_TO_EDGE,
	REPEAT,
	MIRROR_REPEAT,
	CLAMP_TO_BORDER
);

ENUM(FilterMode,
	NEAREST,
	LINEAR
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
	SINT_16X2,
	SINT_16X4,
	UNORM_16X2,
	UNORM_16X4,
	SNORM_16X2,
	SNORM_16X4,
	FLOAT_16X2,
	FLOAT_16X4,
	FLOAT_32,
	FLOAT_32X2,
	FLOAT_32X3,
	FLOAT_32X4,
	UINT_32,
	UINT_32X2,
	UINT_32X3,
	UINT_32X4,
	SINT_32,
	SINT_32X2,
	SINT_32X3,
	SINT_32X4,
);

ENUM(BlendFactor,
	ZERO,
	ONE,
	SRC,
	ONE_MINUS_SRC,
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
	DST,
	ONE_MINUS_DST,
	DST_ALPHA,
	ONE_MINUS_DST_ALPHA,
	SRC_ALPHA_SATURATED,
	CONSTANT,
	ONE_MINUS_CONSTANT
);

ENUM(BlendOperation,
	ADD,
	SUBTRACT,
	REVERSE_SUBTRACT,
	MIN,
	MAX
);

ENUM(BindingType,
	UNIFORM_BUFFER,
	STORAGE_BUFFER,
	SAMPLER,
	TEXTURE,
	STORAGE_TEXTURE,
	ACCELERATION_STRUCTURE
);

ENUM(BindingResourceType,
	BUFFER,
	BUFFER_ARRAY,
	SAMPLER,
	SAMPLER_ARRAY,
	TEXTURE_VIEW,
	TEXTURE_VIEW_ARRAY
);

ENUM(TextureSampleType,
	FLOAT,
	DEPTH,
	UINT,
	SINT
);

ENUM(TextureViewDimension,
	D1,
	D2,
	D2_ARRAY,
	CUBE,
	CUBE_ARRAY,
	D3
);

ENUM(TextureDimension,
	D1,
	D2,
	D3
);

ENUM(LoadOp,
	CLEAR,
	LOAD
);

ENUM(StoreOp,
	STORE,
	DISCARD
);

ENUM(IndexFormat,
	UINT16,
	UINT32
);

struct Extend3D
{
	u32 width;
	u32 height;
	u32 depthOrArrayLayers = 1;
};

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

struct BlendComponent
{
	static BlendComponent Replace()
	{
		BlendComponent component{};
		component.srcFactor = BlendFactor::ONE;
		component.dstFactor = BlendFactor::ZERO;
		return component;
	}

	static BlendComponent AlphaBlending()
	{
		BlendComponent component{};
		component.srcFactor = BlendFactor::SRC_ALPHA;
		component.dstFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
		return component;
	}

	BlendFactor srcFactor;
	BlendFactor dstFactor;
	BlendOperation operation = BlendOperation::ADD;
};

struct BlendState
{
	BlendComponent color;
	BlendComponent alpha;
};

struct ColorTargetState
{
	TextureFormat format = TextureFormat::RGBA8_UNORM_SRGB;
	Optional<BlendState> blend = Optional<BlendState>::None();
};

struct BindingTypeDescriptor
{
	BindingType type;

	union
	{
		struct
		{
			u32 _padding[4];
		} uniformBuffer;

		struct
		{
			b8 readOnly = true;
			u32 _padding[3];
		} storageBuffer;

		struct
		{
			u32 _padding[4];
		} sampler;

		struct
		{
			TextureSampleType sampleType = TextureSampleType::FLOAT;
			TextureViewDimension viewDimension = TextureViewDimension::D2;
			b8 multisampled = false;
			u32 _padding[1];
		} texture;

		struct
		{
			StorageTextureAccess access = StorageTextureAccess::READ;
			TextureFormat format = TextureFormat::RGBA8_UNORM_SRGB;
			TextureViewDimension viewDimension = TextureViewDimension::D2;
			u32 _padding[1];
		} storageTexture;

		struct
		{
			u32 _padding[4];
		} accelerationStructure;
	};
};

struct BindGroupLayoutEntry
{
	u32 binding;
	ShaderStageFlags visibility;
	BindingTypeDescriptor type;
	Optional<u32> count = Optional<u32>::None();
};

struct BufferBinding
{
	HBuffer buffer = HBuffer::null;
	u64 offset = 0;
	Optional<u64> size = Optional<u64>::None();
};

struct BindingResource
{
	BindingResourceType type;

	union
	{
		BufferBinding buffer;
		List<BufferBinding> bufferArray;
		HSampler sampler;
		List<HSampler> samplerArray;
		HTextureView textureView;
		List<HTextureView> textureViewArray;
	};
};

struct BindGroupEntry
{
	u32 binding;
	BindingResource resource;
};

struct BindGroupCreateInfo
{
	Optional<String> name = Optional<String>::None();

	HBindGroupLayout layout = HBindGroupLayout::null;
	List<BindGroupEntry> entries = List<BindGroupEntry>{};
};

struct PipelineLayoutDescriptor
{
	List<BindGroupLayoutEntry> bindGroupLayouts = List<BindGroupLayoutEntry>{};
};

struct GraphicsPipelineCreateInfo
{
	Optional<String> name = Optional<String>::None();

	HShader vertexShader = HShader::null;
	HShader fragmentShader = HShader::null;
	// TODO
	// HShader geometryShader = HShader::null;
	// HShader tessalationShader = HShader::null;

	List<VertexBufferLayout> vertexBuffers = List<VertexBufferLayout>{};
	Optional<ColorTargetState> colorTarget = Optional<ColorTargetState>::None();
	GraphicsPipelineTopology topology = GraphicsPipelineTopology::TRIANGLE_LIST;
	Optional<GraphicsPipelineFaceCull> faceCull = Optional<GraphicsPipelineFaceCull>::None();
	PipelineLayoutDescriptor layout;
	Optional<TextureFormat> depthFormat = Optional<TextureFormat>::None();
};

struct ComputePipelineCreateInfo
{
	Optional<String> name = Optional<String>::None();

	HShader shader = HShader::null;
	PipelineLayoutDescriptor layout;
	HashMap<String, f64> constants = HashMap<String, f64>{};
};

struct ShaderCreateInfo
{
	Optional<String> name = Optional<String>::None();

	ShaderType shaderType;
	String src;
};

struct BufferCreateInfo
{
	Optional<String> name = Optional<String>::None();

	BufferUsageFlags usageFlags = 0;
	u64 size;
};

struct SamplerCreateInfo
{
	Optional<String> name = Optional<String>::None();

	AddressMode addressModeU = AddressMode::CLAMP_TO_EDGE;
	AddressMode addressModeV = AddressMode::CLAMP_TO_EDGE;
	AddressMode addressModeW = AddressMode::CLAMP_TO_EDGE;
	FilterMode magFilter;
	FilterMode minFilter;
	f32 lodMinClamp = 0.0f;
	f32 lodMaxClamp = 32.0f;
	u16 anisotropyClamp = 1;
	Optional<SamplerBorderColor> borderColor = Optional<SamplerBorderColor>::None();
};

struct TextureCreateInfo
{
	Optional<String> name = Optional<String>::None();

	Extend3D size = Extend3D{};
	u32 mipLevelCount = 1;
	u32 sampleCount = 1;
	TextureDimension dimension = TextureDimension::D2;
	TextureFormat format = TextureFormat::RGBA8_UNORM_SRGB;
	TextureUsageFlags usageFlags = 0;
};

struct ImageDataLayout
{
	u64 offset;
	// This value is required if there are multiple rows (i.e. height or depth is more than one pixel or pixel block for compressed textures)
	Optional<u32> bytesPerRow = Optional<u32>::None();
	// Required if there are multiple images (i.e. the depth is more than one).
	Optional<u32> rowsPerImage = Optional<u32>::None();
};

struct BufferSlice
{
	HBuffer buffer = HBuffer::null;
	u64 offset = 0;
	Optional<u64> size = Optional<u64>::None();
};

struct Operations
{
	LoadOp load = LoadOp::LOAD;
	StoreOp store = StoreOp::STORE;
};

struct RenderPassColorAttachment
{
	HTextureView view;
	Optional<HTextureView> resolveTarget = Optional<HTextureView>::None();
	Operations ops = Operations{};
};

struct RenderPassDepthStencilAttachment
{
	HTextureView view;
	Optional<Operations> depthOps = Optional<Operations>::None();
	Optional<Operations> stencilOps = Optional<Operations>::None();
};

struct RenderPassDescriptor
{
	Optional<String> name = Optional<String>::None();

	List<RenderPassColorAttachment> colorAttachments = List<RenderPassColorAttachment>{};
	Optional<RenderPassColorAttachment> depthStencilAttachment = Optional<RenderPassColorAttachment>::None();
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