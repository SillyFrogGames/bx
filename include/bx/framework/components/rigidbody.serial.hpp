#pragma once

#include "bx/framework/components/rigidbody.hpp"

#include <bx/engine/core/serial.serial.hpp>

template <>
struct Serial<RigidBody>
{
	template <class Archive>
	static void Save(Archive& ar, RigidBody const& data)
	{
		try
		{
		}
		catch (cereal::Exception& e)
		{
			ENGINE_LOGW("Failed to save rigidbody component: {}", e.what());
		}
	}

	template <class Archive>
	static void Load(Archive& ar, RigidBody& data)
	{
		try
		{
		}
		catch (cereal::Exception& e)
		{
			ENGINE_LOGW("Failed to load rigidbody component: {}", e.what());
		}
	}
};

REGISTER_POLYMORPHIC_SERIAL(ComponentBase, RigidBody)