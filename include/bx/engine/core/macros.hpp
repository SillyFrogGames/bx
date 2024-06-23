#pragma once

#include "bx/engine/core/byte_types.hpp"
#include "bx/engine/core/log.hpp"

#include <cstdlib>

#define ENGINE_STR(x) #x
#define ENGINE_XSTR(x) ENGINE_STR(x)

#define ENGINE_ARRAYSIZE(_ARR) ((u32)(sizeof(_ARR) / sizeof(*(_ARR))))

#define ENGINE_BIT(x) (1 << (x))

#ifdef DEBUG_BUILD
#define ENGINE_LOGD(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_DEBUG, Log::Format(format, __VA_ARGS__))
#define ENGINE_LOGI(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_INFO, Log::Format(format, __VA_ARGS__))
#define ENGINE_LOGW(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_WARNING, Log::Format(format, __VA_ARGS__))
#define ENGINE_LOGE(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_ERROR, Log::Format(format, __VA_ARGS__))

#define ENGINE_ASSERT(condition, message) \
    do { \
        if (!(condition)) \
        { \
			ENGINE_LOGE("Assertion failed: {}", message); \
            std::abort(); \
        } \
    } while (false)

#define ENGINE_ENSURE(condition) ENGINE_ASSERT(condition, #condition)
#define ENGINE_FAIL(message) ENGINE_ASSERT(false, message)

#else

#define ENGINE_LOGD(format, ...)
#define ENGINE_LOGI(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_INFO, Log::Format(format, __VA_ARGS__))
#define ENGINE_LOGW(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_WARNING, Log::Format(format, __VA_ARGS__))
#define ENGINE_LOGE(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_ERROR, Log::Format(format, __VA_ARGS__))

#define ENGINE_ASSERT(condition, message)
#define ENGINE_ENSURE(condition)
#define ENGINE_FAIL(message)

#endif