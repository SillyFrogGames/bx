#pragma once

#include <memory>

#include "hash_map.hpp"

// Lazily initialize a const T on demand
template <typename InitializerT, typename T>
struct LazyInit : NoCopy
{
	virtual ~LazyInit() {}

    template <typename ...Params>
	static const T& Get(Params&&... params)
    {
        if (!cache)
        {
            cache = std::make_unique<InitializerT>(std::forward<Params>(params)...);
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

// Lazily initialize a const T on demand but caches based on initializer args
template <typename InitializerT, typename T, typename InitArgsT>
struct LazyInitMap : NoCopy
{
    virtual ~LazyInitMap() {}

    static const T& Get(const InitArgsT& params)
    {
        auto& cacheIter = cache.find(params);
        if (cacheIter == cache.end())
        {
            cache.try_emplace(params, new InitializerT(params));
        }

        return cache.find(params)->second->data;
    }

    static void Clear()
    {
        cache.clear();
    }

protected:
    T data;

private:
    static HashMap<InitArgsT, std::unique_ptr<InitializerT>> cache;
};