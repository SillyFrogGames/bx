#include "bx/engine/modules/graphics/type.hpp"

#include "bx/engine/modules/graphics/type_validation.hpp"

const BufferHandle BufferHandle::null = { 0 };
const TextureHandle TextureHandle::null = { 0 };
const TextureViewHandle TextureViewHandle::null = { 0 };
const ShaderHandle ShaderHandle::null = { 0 };
const GraphicsPipelineHandle GraphicsPipelineHandle::null = { 0 };
const ComputePipelineHandle ComputePipelineHandle::null = { 0 };
const RenderPassHandle RenderPassHandle::null = { 0 };
const BindGroupLayoutHandle BindGroupLayoutHandle::null = { 0 };
const BindGroupHandle BindGroupHandle::null = { 0 };

BlendComponent BlendComponent::Replace()
{
	BlendComponent component{};
	component.srcFactor = BlendFactor::ONE;
	component.dstFactor = BlendFactor::ZERO;
	return component;
}

BlendComponent BlendComponent::AlphaBlending()
{
	BlendComponent component{};
	component.srcFactor = BlendFactor::SRC_ALPHA;
	component.dstFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
	return component;
}

BindingTypeDescriptor BindingTypeDescriptor::UniformBuffer()
{
	BindingTypeDescriptor descriptor{};
	descriptor.type = BindingType::UNIFORM_BUFFER;
	return descriptor;
}

BindingTypeDescriptor BindingTypeDescriptor::StorageBuffer(b8 readOnly)
{
	BindingTypeDescriptor descriptor{};
	descriptor.type = BindingType::STORAGE_BUFFER;
	descriptor.storageBuffer.readOnly = readOnly;
	return descriptor;
}

BindingTypeDescriptor BindingTypeDescriptor::Sampler()
{
	BindingTypeDescriptor descriptor{};
	descriptor.type = BindingType::SAMPLER;
	return descriptor;
}

BindingTypeDescriptor BindingTypeDescriptor::Texture(TextureSampleType sampleType, TextureViewDimension viewDimension, b8 multisampled)
{
	BindingTypeDescriptor descriptor{};
	descriptor.type = BindingType::TEXTURE;
	descriptor.texture.sampleType = sampleType;
	descriptor.texture.viewDimension = viewDimension;
	descriptor.texture.multisampled = multisampled;
	return descriptor;
}

BindingTypeDescriptor BindingTypeDescriptor::StorageTexture(StorageTextureAccess access, TextureFormat format, TextureViewDimension viewDimension)
{
	BX_ASSERT(!IsTextureFormatSrgb(format), "Storage texture format cannot be srgb.");

	BindingTypeDescriptor descriptor{};
	descriptor.type = BindingType::STORAGE_TEXTURE;
	descriptor.storageTexture.access = access;
	descriptor.storageTexture.format = format;
	descriptor.storageTexture.viewDimension = viewDimension;
	return descriptor;
}

BindingTypeDescriptor BindingTypeDescriptor::AccelerationStructure()
{
	BindingTypeDescriptor descriptor{};
	descriptor.type = BindingType::ACCELERATION_STRUCTURE;
	return descriptor;
}

BindingResource BindingResource::Buffer(const BufferBinding& bufferBinding)
{
	BindingResource resource{};
	resource.type = BindingResourceType::BUFFER;
	resource.buffer = bufferBinding;
	return resource;
}

BindingResource BindingResource::BufferArray(const List<BufferBinding>& bufferBindings)
{
	BindingResource resource{};
	resource.type = BindingResourceType::BUFFER_ARRAY;
	resource.bufferArray = bufferBindings;
	return resource;
}

BindingResource BindingResource::Sampler(const SamplerHandle& sampler)
{
	BindingResource resource{};
	resource.type = BindingResourceType::SAMPLER;
	resource.sampler = sampler;
	return resource;
}

BindingResource BindingResource::SamplerArray(const List<SamplerHandle>& samplers)
{
	BindingResource resource{};
	resource.type = BindingResourceType::SAMPLER_ARRAY;
	resource.samplerArray = samplers;
	return resource;
}

BindingResource BindingResource::TextureView(const TextureViewHandle& textureView)
{
	BindingResource resource{};
	resource.type = BindingResourceType::TEXTURE_VIEW;
	resource.textureView = textureView;
	return resource;
}

BindingResource BindingResource::TextureViewArray(const List<TextureViewHandle>& textureViews)
{
	BindingResource resource{};
	resource.type = BindingResourceType::TEXTURE_VIEW_ARRAY;
	resource.textureViewArray = textureViews;
	return resource;
}

b8 IsVertexFormatInt(const VertexFormat& format)
{
	switch (format)
	{
	case VertexFormat::UINT_8X2:
		return true;
	case VertexFormat::UINT_8X4:
		return true;
	case VertexFormat::SINT_8X2:
		return true;
	case VertexFormat::SINT_8X4:
		return true;
	case VertexFormat::UNORM_8X2:
		return false;
	case VertexFormat::UNORM_8X4:
		return false;
	case VertexFormat::SNORM_8X2:
		return false;
	case VertexFormat::SNORM_8X4:
		return false;

	case VertexFormat::UINT_16X2:
		return true;
	case VertexFormat::UINT_16X4:
		return true;
	case VertexFormat::SINT_16X2:
		return true;
	case VertexFormat::SINT_16X4:
		return true;
	case VertexFormat::UNORM_16X2:
		return false;
	case VertexFormat::UNORM_16X4:
		return false;
	case VertexFormat::SNORM_16X2:
		return false;
	case VertexFormat::SNORM_16X4:
		return false;
	case VertexFormat::FLOAT_16X2:
		return false;
	case VertexFormat::FLOAT_16X4:
		return false;

	case VertexFormat::FLOAT_32:
		return false;
	case VertexFormat::FLOAT_32X2:
		return false;
	case VertexFormat::FLOAT_32X3:
		return false;
	case VertexFormat::FLOAT_32X4:
		return false;
	case VertexFormat::UINT_32:
		return true;
	case VertexFormat::UINT_32X2:
		return true;
	case VertexFormat::UINT_32X3:
		return true;
	case VertexFormat::UINT_32X4:
		return true;
	case VertexFormat::SINT_32:
		return true;
	case VertexFormat::SINT_32X2:
		return true;
	case VertexFormat::SINT_32X3:
		return true;
	case VertexFormat::SINT_32X4:
		return true;
	default:
		BX_FAIL("Vertex format not supported.");
	}
}

b8 IsTextureFormatSrgb(const TextureFormat& format)
{
	switch (format)
	{
	case TextureFormat::RGBA8_UNORM_SRGB:
		return true;
	case TextureFormat::BGRA8_UNORM_SRGB:
		return true;
	case TextureFormat::BC1_RGBA_UNORM_SRGB:
		return true;
	case TextureFormat::BC2_RGBA_UNORM_SRGB:
		return true;
	case TextureFormat::BC3_RGBA_UNORM_SRGB:
		return true;
	case TextureFormat::BC7_RGBA_UNORM_SRGB:
		return true;
	default:
		return false;
	}
}