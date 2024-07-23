#pragma once

#include "bx/engine/core/byte_types.hpp"
#include "bx/engine/core/macros.hpp"

#include <memory>
#include <utility>

template <typename T>
class Optional
{
public:
	Optional()
		: data(nullptr)
	{}

	Optional(const Optional<T>& other)
	{
		if (other.IsSome())
			data = new T(*other.data);
		else
			data = nullptr;
	}

	Optional<T>& operator=(const Optional<T>& other)
	{
		if (other.IsSome())
			data = new T(*other.data);
		else
			data = nullptr;
		return *this;
	}

	Optional(Optional<T>&& other) noexcept
	{
		data = other.data;
		other.data = nullptr;
	}

	Optional<T>& operator=(Optional<T>&& other) noexcept
	{
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	~Optional()
	{
		delete data;
	}

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
		return data;
	}

	b8 IsNone() const
	{
		return !data;
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
	template <typename ...Params>
	Optional(Params&&... params)
		: data(new T(std::forward<Params>(params)...))
	{}

	T* data;
};