#pragma once

#include "bx/engine/core/byte_types.hpp"
#include "bx/engine/core/log.hpp"

#include <cstdlib>

#define BX_STR(x) #x
#define BX_XSTR(x) BX_STR(x)

#define BX_ARRAYSIZE(_ARR) ((u32)(sizeof(_ARR) / sizeof(*(_ARR))))

#define BX_BIT(x) (1 << (x))

#ifdef BX_DEBUG_BUILD
#define BX_LOGD(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_DEBUG, Log::Format(format, __VA_ARGS__))
#define BX_LOGI(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_INFO, Log::Format(format, __VA_ARGS__))
#define BX_LOGW(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_WARNING, Log::Format(format, __VA_ARGS__))
#define BX_LOGE(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_ERROR, Log::Format(format, __VA_ARGS__))

#define BX_ASSERT(condition, message) \
    do { \
        if (!(condition)) \
        { \
			BX_LOGE("Assertion failed: {}", message); \
            std::abort(); \
        } \
    } while (false)

#define BX_ENSURE(condition) BX_ASSERT(condition, #condition)
#define BX_FAIL(message) BX_ASSERT(false, message)

#else

#define BX_LOGD(format, ...)
#define BX_LOGI(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_INFO, Log::Format(format, __VA_ARGS__))
#define BX_LOGW(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_WARNING, Log::Format(format, __VA_ARGS__))
#define BX_LOGE(format, ...) Log::Print(__FILE__, __LINE__, __func__, LogLevel::LOG_ERROR, Log::Format(format, __VA_ARGS__))

#define BX_ASSERT(condition, message)
#define BX_ENSURE(condition)
#define BX_FAIL(message)

#endif