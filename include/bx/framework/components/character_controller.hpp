#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>
#include <bx/engine/modules/physics.hpp>

class CharacterController : public Component<CharacterController>
{
public:
	CharacterController();

	void OnPostCopy() override
	{
		m_isDirty = true;
		m_characterController = PHYSICS_INVALID_HANDLE;
	}

	void OnRemoved() override
	{
		if (m_characterController != PHYSICS_INVALID_HANDLE)
			Physics::DestroyCharacterController(m_characterController);
	}

	inline Vec3 GetOffset() const { return m_offset; }
	inline void SetOffset(const Vec3& offset) { m_offset = offset; m_isDirty = true; }

	inline f32 GetWidth() const { return m_width; }
	inline void SetWidth(f32 width) { m_width = width; m_isDirty = true; }

	inline f32 GetHeight() const { return m_height; }
	inline void SetHeight(f32 height) { m_height = height; m_isDirty = true; }

	inline f32 GetStepHeight() const { return m_stepHeight; }
	inline void SetStepHeight(f32 stepHeight) { m_stepHeight = stepHeight; m_isDirty = true; }

	inline PhysicsHandle GetCharacterController() const { return m_characterController; }

	inline void SetMoveVector(const Vec3& move) const
	{
		Physics::SetCharacterControllerMove(m_characterController, move);
	}

	inline void SetLinearVelocity(const Vec3& velocity) const
	{
		Physics::SetCharacterControllerVelocity(m_characterController, velocity);
	}

	inline void SetRotation(const Quat& rotation) const
	{
		Physics::SetCharacterControllerRotation(m_characterController, rotation);
	}

	inline void ApplyImpulse(const Vec3& impulse) const
	{
		Physics::CharacterControllerApplyImpulse(m_characterController, impulse);
	}

	inline void Build(const Mat4& matrix)
	{
		if (!m_isDirty)
			return;

		m_isDirty = false;

		if (m_characterController != PHYSICS_INVALID_HANDLE)
			Physics::DestroyCharacterController(m_characterController);

		CharacterControllerInfo info;
		info.matrix = matrix;
		info.offset = m_offset;
		info.up = Vec3(0, 1, 0);
		info.height = m_height;
		info.width = m_width;
		info.stepHeight = m_stepHeight;
		info.id = GetEntity().GetId();
		m_characterController = Physics::CreateCharacterController(info);
	}

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;

	bool m_isDirty = true;

	Vec3 m_offset = Vec3(0, 0, 0);
	f32 m_width = 0.5f;
	f32 m_height = 1.8f;

	f32 m_stepHeight = 0.35f;

	PhysicsHandle m_characterController = PHYSICS_INVALID_HANDLE;
};