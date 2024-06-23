#pragma once

#include "bx/framework/components/collider.hpp"

#include "bx/framework/resources/mesh.serial.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/type.serial.hpp>
#include <bx/engine/core/math.serial.hpp>
#include <bx/engine/core/resource.serial.hpp>

template <>
struct Serial<Collider>
{
	template <class Archive>
	static void Save(Archive& ar, Collider const& data)
	{
		ar(cereal::make_nvp("shape", data.m_shape));
		ar(cereal::make_nvp("center", data.m_center));
		ar(cereal::make_nvp("size", data.m_size));
		ar(cereal::make_nvp("radius", data.m_radius));
		ar(cereal::make_nvp("height", data.m_height));
		ar(cereal::make_nvp("axis", data.m_axis));
		ar(cereal::make_nvp("scale", data.m_scale));
		ar(cereal::make_nvp("isConcave", data.m_isConcave));
		ar(cereal::make_nvp("mesh", data.m_mesh));
	}

	template <class Archive>
	static void Load(Archive& ar, Collider& data)
	{
		ar(cereal::make_nvp("shape", data.m_shape));
		ar(cereal::make_nvp("center", data.m_center));
		ar(cereal::make_nvp("size", data.m_size));
		ar(cereal::make_nvp("radius", data.m_radius));
		ar(cereal::make_nvp("height", data.m_height));
		ar(cereal::make_nvp("axis", data.m_axis));
		ar(cereal::make_nvp("scale", data.m_scale));
		ar(cereal::make_nvp("isConcave", data.m_isConcave));
		ar(cereal::make_nvp("mesh", data.m_mesh));
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, Collider)