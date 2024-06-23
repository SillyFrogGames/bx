#pragma once

#include "bx/framework/components/mesh_filter.hpp"

#include "bx/framework/resources/mesh.serial.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/resource.serial.hpp>
#include <bx/engine/containers/list.serial.hpp>

template <>
struct Serial<MeshFilter>
{
	template <class Archive>
	static void Save(Archive& ar, MeshFilter const& data)
	{
		ar(cereal::make_nvp("meshes", data.m_meshes));
	}

	template <class Archive>
	static void Load(Archive& ar, MeshFilter& data)
	{
		ar(cereal::make_nvp("meshes", data.m_meshes));
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, MeshFilter)