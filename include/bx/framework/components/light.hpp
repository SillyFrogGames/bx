#pragma once

#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/math.hpp>

class Light : public Component<Light>
{
public:
	Light();

	inline f32 GetIntensity() const { return m_intensity; }
	inline void SetIntensity(f32 intensity) { m_intensity = intensity; }

	inline f32 GetConstant() const { return m_constant; }
	inline void SetConstant(f32 constant) { m_constant = constant; }

	inline f32 GetLinear() const { return m_linear; }
	inline void SetLinear(f32 linear) { m_linear = linear; }

	inline f32 GetQuadratic() const { return m_quadratic; }
	inline void SetQuadratic(f32 quadratic) { m_quadratic = quadratic; }

	inline f32 GetCutOff() const { return m_cutoff; }
	inline void SetCutOff(f32 cutoff) { m_cutoff = cutoff; }

	inline f32 GetOuterCutOff() const { return m_outerCutoff; }
	inline void SetOuterCutOff(f32 outerCutoff) { m_outerCutoff = outerCutoff; }

	inline const Vec3& GetColor() const { return m_color; }
	inline void SetColor(const Vec3& color) { m_color = color; }

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Inspector;

	// type

	f32 m_intensity = 1.0f;

	f32 m_constant = 1.0f;
	f32 m_linear = 0.1f;
	f32 m_quadratic = 0.01f;

	f32 m_cutoff = 0.0f;
	f32 m_outerCutoff = 0.0f;

	Vec3 m_color = Vec3(1, 1, 1);
};