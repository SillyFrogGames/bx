#pragma once

#include "bx/engine/core/byte_types.hpp"
#include "bx/engine/core/macros.hpp"

#include <memory>
#include <utility>

template <typename T>
class Optional
{
public:
	template <typename ...Params>
	static Optional<T> Some(Params&&... params)
	{
		return Optional(std::forward<Params>(params)...);
	}

	static Optional<T> None()
	{
		return Optional{};
	}

	b8 IsSome() const
	{
		return data != nullptr;
	}

	b8 IsNone() const
	{
		return data == nullptr;
	}

	T& Unwrap()
	{
		BX_ASSERT(data, "Unwrap on a None value.");
		return *data;
	}

	const T& Unwrap() const
	{
		BX_ASSERT(data, "Unwrap on a None value.");
		return *data;
	}

private:
	Optional()
		: data(nullptr)
	{}

	template <typename ...Params>
	Optional(Params&&... params)
		: data(new T(std::forward<Params>(params)...))
	{}

	// TODO: Optional<T> does not support copying, even if T itself does
	// Replacing unique_ptr with a raw ptr and calling copy constructors when relevant can add the ability to copy Optional<T> when T implements copy constructors
	std::unique_ptr<T> data;
};