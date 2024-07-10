#pragma once

#include "type.hpp"

template <typename T>
struct Handle
{
	u64 id;
	static const Handle<T> null;

	b8 operator==(const Handle<T>& other) const { return id == other.id; }
	operator b8() const { return id != 0; }
};

template <typename T>
const Handle<T> Handle<T>::null = { 0 };

template <typename T>
struct std::hash<Handle<T>>
{
	std::size_t operator()(const Handle<T>& h) const
	{
		return std::hash<u64>()(h.id);
	}
};