#pragma once

#include "bx/framework/components/attributes.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/math.serial.hpp>

template <>
struct Serial<Attributes>
{
	template <class Archive>
	static void Save(Archive& ar, Attributes const& data)
	{
	}

	template <class Archive>
	static void Load(Archive& ar, Attributes& data)
	{
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, Attributes)