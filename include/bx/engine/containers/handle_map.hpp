#pragma once

#include "bx/engine/core/byte_types.hpp"
#include "bx/engine/core/type.hpp"
#include "bx/engine/core/macros.hpp"
#include "bx/engine/core/memory.hpp"
#include "bx/engine/containers/list.hpp"

// See:
// https://bitsquid.blogspot.com/2011/09/managing-decoupling-part-4-id-lookup.html
// https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/game-engine-containers-handle_map-r4495/

struct HandleId
{
	inline bool operator==(const HandleId& other) const { return id == other.id; }
	inline bool operator!=(const HandleId& other) const { return id != other.id; }
	inline bool operator<(const HandleId& other) const { return id < other.id; }
	inline bool operator>(const HandleId& other) const { return id > other.id; }

	union
	{
		u64 id;
		struct
		{
			u32 index;
			u16 generation;
			u16 uuid;
		};
	};
};

template <typename TVal>
class Handle
{
public:
	Handle() = default;
	Handle(HandleId id) : m_id(id) {}

	inline bool operator==(const Handle& other) const { return m_id == other.m_id; }
	inline bool operator!=(const Handle& other) const { return m_id != other.m_id; }
	inline bool operator<(const Handle& other) const { return m_id < other.m_id; }
	inline bool operator>(const Handle& other) const { return m_id > other.m_id; }

	inline HandleId GetId() const { return m_id; }

private:
	HandleId m_id;
};

struct HandleIndex
{
	u32 innerIndex = 0; // From index to object
	u32 outerIndex = 0; // From object to index

	union
	{
		u32 id;
		struct
		{
			u16 generation;
			u16 meta : 15;
			u16 used : 1;
		};
	};
};

template <typename TVal>
class HandleMap
{
public:
	explicit HandleMap(SizeType capacity = 100)
	{
		m_indices.resize(capacity);
		m_objects.resize(capacity);

		for (SizeType i = 0; i < capacity - 1; ++i)
		{
			auto& in = m_indices[i];

			in.innerIndex = (u32)i + 1;
			in.outerIndex = 0xFFFFFFFF;

			in.id = 0;
		}

		m_freeListDequeue = 0;
		m_freeListEnqueue = capacity - 1;
	}

	inline Handle<TVal> Insert(const TVal& val)
	{
		auto index = m_freeListDequeue;
		auto& in = m_indices[index];
		m_freeListDequeue = in.innerIndex;

		in.innerIndex = m_count++;
		in.generation++;
		in.used = 1;
		
		m_indices[in.innerIndex].outerIndex = index;
		m_objects[in.innerIndex] = val;
		
		HandleId id{};
		id.index = index;
		id.generation = in.generation;
		id.uuid = GetTypeId();

		return Handle<TVal>(id);
	}

	inline bool Remove(Handle<TVal> handle)
	{
		if (!IsValid(handle))
			return false;

		const auto handleId = handle.GetId();

		auto& in = m_indices[handleId.index];
		in.used = 0;

		auto lastIndex = --m_count;
		m_objects[in.innerIndex] = std::move(m_objects[lastIndex]); // Move last object to empty slot
		m_indices[m_indices[lastIndex].outerIndex].innerIndex = in.innerIndex; // Update last object index to use new inner index
		
		// Set inner index to the next index
		m_indices[m_freeListEnqueue].innerIndex = handleId.index;
		m_freeListEnqueue = handleId.index;

		return true;
	}

	inline TVal& Get(Handle<TVal> handle)
	{
		auto innerIndex = GetInnerIndex(handle);
		return m_objects[innerIndex];
	}
	
	inline const TVal& Get(Handle<TVal> handle) const
	{
		auto innerIndex = GetInnerIndex(handle);
		return m_objects[innerIndex];
	}
	
	inline bool IsValid(Handle<TVal> handle)
	{
		const auto handleId = handle.GetId();
	
		if (handleId.index >= m_indices.size())
			return false;
	
		auto& in = m_indices[handleId.index];
	
		return handleId.uuid == GetTypeId()
			&& handleId.generation == in.generation
			&& in.innerIndex < m_count;
	}

private:
	inline u16 GetTypeId() const { return Type<TVal>::Id() % 0xFFFF; }

	inline u32 GetInnerIndex(Handle<TVal> handle) const
	{
		const auto handleId = handle.GetId();
	
		BX_ASSERT(handleId.index < m_indices.size(), "Outer index out of range!");
	
		const auto& in = m_indices[handleId.index];
	
		BX_ASSERT(handleId.uuid == GetTypeId(), "TypeId mismatch!");
		BX_ASSERT(handleId.generation == in.generation, "At called with old generation!");
		BX_ASSERT(in.innerIndex < m_count, "Inner index out of range!");
	
		return in.innerIndex;
	}

private:
	u32 m_count = 0;
	u32 m_freeListEnqueue = 0;
	u32 m_freeListDequeue = 0;

	List<HandleIndex> m_indices;
	List<TVal> m_objects;
};