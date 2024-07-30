#pragma once

#include <memory>

template<typename InitializerT, typename T>
struct LazyInit : NoCopy
{
	virtual ~LazyInit() {}

	static const T& Get()
    {
        if (!cache)
        {
            cache = std::make_unique<InitializerT>();
        }

        return cache->data;
    }

	static void Clear()
    {
        cache.reset();
    }

protected:
    T data;

private:
    static std::unique_ptr<InitializerT> cache;
};