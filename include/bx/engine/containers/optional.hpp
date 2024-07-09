#pragma once

#include "bx/engine/core/byte_types.hpp"
#include "bx/engine/core/macros.hpp"

#include <memory>
#include <utility>

template <typename T>
class Optional
{
public:
	Optional(const Optional<T>& other) = default;
	Optional& operator=(const Optional<T>& other) = default;
	Optional(Optional<T>&& other) = default;
	Optional& operator=(Optional<T>&& other) = default;

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
		return isSome;
	}

	b8 IsNone() const
	{
		return !isSome;
	}

	T& Unwrap()
	{
		BX_ASSERT(isSome, "Unwrap on a None value.");
		return data;
	}

	const T& Unwrap() const
	{
		BX_ASSERT(isSome, "Unwrap on a None value.");
		return data;
	}

private:
	Optional()
		: isSome(false)
	{}

	template <typename ...Params>
	Optional(Params&&... params)
		: data(std::forward<Params>(params)...), isSome(true)
	{}

	// TODO: Optional<T> does not support copying, even if T itself does
	// Replacing unique_ptr with a raw ptr and calling copy constructors when relevant can add the ability to copy Optional<T> when T implements copy constructors
	// std::unique_ptr<T> data;

	T data;
	b8 isSome;
};