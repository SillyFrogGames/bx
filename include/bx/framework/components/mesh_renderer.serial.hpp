#pragma once

#include "bx/framework/components/mesh_renderer.hpp"

#include "bx/framework/resources/material.serial.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/resource.serial.hpp>
#include <bx/engine/containers/list.serial.hpp>

template <>
struct Serial<MeshRenderer>
{
	template <class Archive>
	static void Save(Archive& ar, MeshRenderer const& data)
	{
		ar(cereal::make_nvp("materials", data.m_materials));

		ar(cereal::make_nvp("shadowCastingMode", (int)data.m_shadowCastingMode));
		ar(cereal::make_nvp("receiveShadows", data.m_receiveShadows));
		ar(cereal::make_nvp("contributeGI", data.m_contributeGI));
		ar(cereal::make_nvp("receiveGI", data.m_receiveGI));
	}

	template <class Archive>
	static void Load(Archive& ar, MeshRenderer& data)
	{
		ar(cereal::make_nvp("materials", data.m_materials));

		ar(cereal::make_nvp("shadowCastingMode", (int&)data.m_shadowCastingMode));
		ar(cereal::make_nvp("receiveShadows", data.m_receiveShadows));
		ar(cereal::make_nvp("contributeGI", data.m_contributeGI));
		ar(cereal::make_nvp("receiveGI", data.m_receiveGI));
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, MeshRenderer)