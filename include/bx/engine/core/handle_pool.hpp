#pragma once

#include "type.hpp"
#include "handle.hpp"

#include <queue>

template <typename T>
class HandlePool
{
public:
	HandlePool(u64 size = 1024 * 1024)
	{
		for (u64 i = 1; i < size + 1; i++)
		{
			m_availableHandles.emplace(i);
		}
	}

	Handle<T> Create()
	{
		BX_ASSERT(!m_availableHandles.empty(), "Failed to create handles, pool is emtpy.");

		u64 id = m_availableHandles.pop();
		return Handle<T>{ id };
	}

	void Destroy(Handle<T>& handle)
	{
		BX_ENSURE(handle);

		u64 id = handle.id;
		handle.id = 0;

		m_availableHandles.push(id);
	}

private:
	std::queue<u64> m_availableHandles;
};