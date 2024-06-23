#pragma once

#include "bx/framework/resources/material.hpp"

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/resource.hpp>
#include <bx/engine/containers/list.hpp>

ENUM(ShadowCastingMode, OFF, ON, TWO_SIDED, SHADOWS_ONLY);

class MeshRenderer : public Component<MeshRenderer>
{
public:
	MeshRenderer();

	inline const List<Resource<Material>>& GetMaterials() const { return m_materials; }
	inline SizeType GetMaterialCount() const { return m_materials.size(); }

	inline void AddMaterial(const Resource<Material>& material)
	{
		m_materials.emplace_back(material);
	}

	inline const Resource<Material>& GetMaterial(SizeType index) const
	{
		ENGINE_ENSURE(index < m_materials.size());
		return m_materials[index];
	}

	inline void SetMaterial(SizeType index, const Resource<Material>& material)
	{
		ENGINE_ENSURE(index < m_materials.size());
		m_materials[index] = material;
	}

	inline void RemoveMaterial(SizeType index)
	{
		ENGINE_ENSURE(index < m_materials.size());
		m_materials.erase(m_materials.begin() + index);
	}

	inline ShadowCastingMode GetShadowCastingMode() const { return m_shadowCastingMode; }
	inline void SetShadowCastingMode(ShadowCastingMode mode) { m_shadowCastingMode = mode; }

	inline bool GetReceiveShadows() const { return m_receiveShadows; }
	inline void SetReceiveShadows(bool receiveShadows) { m_receiveShadows = receiveShadows; }

	inline bool GetContributeGI() const { return m_contributeGI; }
	inline void SetContributeGI(bool contributeGI) { m_contributeGI = contributeGI; }

	inline bool GetReceiveGI() const { return m_receiveGI; }
	inline void SetReceiveGI(bool receiveGI) { m_receiveGI = receiveGI; }

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;

	List<Resource<Material>> m_materials;

	ShadowCastingMode m_shadowCastingMode = ShadowCastingMode::ON;
	bool m_receiveShadows = true;
	bool m_contributeGI = true;
	bool m_receiveGI = true;
};