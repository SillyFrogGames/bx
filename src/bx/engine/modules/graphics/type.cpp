#include "bx/engine/modules/graphics/type.hpp"

u64 HBuffer::null = { 0 };
u64 HTexture::null = { 0 };
u64 HTextureView::null = { 0 };
u64 HShader::null = { 0 };
u64 HGraphicsPipeline::null = { 0 };
u64 HComputePipeline::null = { 0 };
u64 HRenderPass::null = { 0 };
u64 HBindGroupLayout::null = { 0 };
u64 HBindGroup::null = { 0 };

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

BindingResource BindingResource::Sampler(const HSampler& sampler)
{
	BindingResource resource{};
	resource.type = BindingResourceType::SAMPLER;
	resource.sampler = sampler;
	return resource;
}

BindingResource BindingResource::SamplerArray(const List<HSampler>& samplers)
{
	BindingResource resource{};
	resource.type = BindingResourceType::SAMPLER_ARRAY;
	resource.samplerArray = samplers;
	return resource;
}

BindingResource BindingResource::TextureView(const HTextureView& textureView)
{
	BindingResource resource{};
	resource.type = BindingResourceType::TEXTURE_VIEW;
	resource.textureView = textureView;
	return resource;
}

BindingResource BindingResource::TextureViewArray(const List<HTextureView>& textureViews)
{
	BindingResource resource{};
	resource.type = BindingResourceType::TEXTURE_VIEW_ARRAY;
	resource.textureViewArray = textureViews;
	return resource;
}