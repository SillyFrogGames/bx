#include "bx/framework/components/collider.hpp"
#include "bx/framework/components/collider.serial.hpp"

Collider::Collider()
{
	// Dummy so compiler doesn't optimize away this source file
}

void Collider::ComputeColliderVertices(const List<Vec3>& inVertices, const List<u32>& inTriangles, List<Vec3>& outVertices) const
{
	if (m_isConcave)
	{
		outVertices.reserve(inTriangles.size());
		for (auto i : inTriangles)
		{
			outVertices.emplace_back(inVertices[i]);
		}
	}
	else
	{
		outVertices.reserve(inVertices.size());
		for (const auto& v : inVertices)
		{
			outVertices.emplace_back(v);
		}
	}
}