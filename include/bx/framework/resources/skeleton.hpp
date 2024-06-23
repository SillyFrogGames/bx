#pragma once

#include <bx/engine/core/math.hpp>
#include <bx/engine/containers/string.hpp>
#include <bx/engine/containers/list.hpp>
#include <bx/engine/containers/hash_map.hpp>
#include <bx/engine/containers/tree.hpp>

#define SKELETON_MAX_BONES 4

class Skeleton
{
public:
	struct Bone
	{
		Mat4 local = Mat4::Identity();
		Mat4 offset = Mat4::Identity();
	};

public:
	Skeleton() {}
	Skeleton(const List<Bone>& bones, const HashMap<String, SizeType>& boneMap, const Tree<String>& boneTree)
		: m_bones(bones)
		, m_boneMap(boneMap)
		, m_boneTree(boneTree)
	{}

	inline const List<Bone>& GetBones() const { return m_bones; }
	inline const HashMap<String, SizeType>& GetBoneMap() const { return m_boneMap; }
	inline const Tree<String>& GetBoneTree() const { return m_boneTree; }

private:
	template <typename T>
	friend class Serial;

	template <typename T>
	friend class Resource;

	List<Bone> m_bones;
	HashMap<String, SizeType> m_boneMap;
	Tree<String> m_boneTree;
};