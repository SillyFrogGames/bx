#pragma once

#include "bx/framework/components/animator.hpp"

#include "bx/framework/resources/skeleton.serial.hpp"
#include "bx/framework/resources/animation.serial.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>

template <>
struct Serial<Animator>
{
	template <class Archive>
	static void Save(Archive& ar, Animator const& data)
	{
		ar(cereal::make_nvp("skeleton", data.m_skeleton));
		ar(cereal::make_nvp("animations", data.m_animations));
	}

	template <class Archive>
	static void Load(Archive& ar, Animator& data)
	{
		ar(cereal::make_nvp("skeleton", data.m_skeleton));
		ar(cereal::make_nvp("animations", data.m_animations));
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, Animator)