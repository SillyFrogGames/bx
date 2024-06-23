#pragma once

#include "bx/framework/gameobject.hpp"

#include <bx/engine/core/serial.serial.hpp>
#include <bx/engine/core/ecs.serial.hpp>

template<class Archive>
void serialize(Archive& ar, GameObjectData& data)
{
	ar(cereal::make_nvp("name", data.name));
	ar(cereal::make_nvp("className", data.className));
	ar(cereal::make_nvp("entity", data.entity));
	ar(cereal::make_nvp("components", data.components));
}

template <>
class Serial<Scene>
{
public:
	template <class Archive>
	static void Save(Archive& ar, const Scene& data)
	{
		List<GameObjectData> gameObjs;
		gameObjs.reserve(data.m_gameObjects.size());
		for (const auto gameObj : data.m_gameObjects)
		{
			GameObjectData gameObjWrapper;
			gameObjWrapper.name = gameObj->m_name;
			gameObjWrapper.className = gameObj->m_className;
			gameObjWrapper.entity = gameObj->m_entity;

			const auto& cmpPtrs = gameObj->m_entity.GetComponents();
			gameObjWrapper.components.reserve(cmpPtrs.size());
			for (const auto cmpPtr : cmpPtrs)
			{
				gameObjWrapper.components.emplace_back(cmpPtr, [](ComponentBase*) {});
			}
			gameObjs.emplace_back(gameObjWrapper);
		}

		ar(cereal::make_nvp("gameObjects", gameObjs));
	}

	template <class Archive>
	static void Load(Archive& ar, Scene& data)
	{
		List<GameObjectData> gameObjs;
		ar(cereal::make_nvp("gameObjects", gameObjs));

		for (const auto& gameObj : gameObjs)
		{
			try
			{
				auto& obj = GameObject::NewFromData(data, gameObj);
				obj.Initialize(gameObj);
			}
			catch (std::exception& e)
			{
				BX_LOGW("Failed to load gameobject: {}. {}", gameObj.className, e.what());
			}
		}
	}
};
REGISTER_SERIAL(Scene);