#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>
#include <bx/engine/modules/physics.hpp>

class RigidBody : public Component<RigidBody>
{
public:
	RigidBody();

	void OnPostCopy() override
	{
		m_isDirty = true;
		m_rigidBody = PHYSICS_INVALID_HANDLE;
	}

	void OnRemoved() override
	{
		if (m_rigidBody != PHYSICS_INVALID_HANDLE)
			Physics::DestroyRigidBody(m_rigidBody);
	}

	//inline f32 GetMass() const { return m_mass; }
	//inline void SetMass(f32 mass) { m_mass = radius; m_isDirty = true; }

	inline PhysicsHandle GetCollider() const { return m_collider; }
	inline void SetCollider(PhysicsHandle collider) { m_collider = collider; m_isDirty = true; }

	inline PhysicsHandle GetRigidBody() const { return m_rigidBody; }

	inline void Build(const Mat4& matrix)
	{
		if (!m_isDirty)
			return;

		m_isDirty = false;

		if (m_rigidBody != PHYSICS_INVALID_HANDLE)
			Physics::DestroyRigidBody(m_rigidBody);

		RigidBodyInfo info;
		info.matrix = matrix;
		info.collider = m_collider;
		info.id = GetEntity().GetId();
		m_rigidBody = Physics::CreateRigidBody(info);
	}

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;

	bool m_isDirty = true;

	PhysicsHandle m_collider = PHYSICS_INVALID_HANDLE;
	PhysicsHandle m_rigidBody = PHYSICS_INVALID_HANDLE;
};