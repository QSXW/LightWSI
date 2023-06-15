/**
 * Copyright (C) 2023, by Wu Jianhua (toqsxw@outlook.com)
 *
 * This library is distributed under the Apache-2.0 license.
 */

#include <vulkan/vulkan.h>

#ifdef _MSC_VER
#define LIGHTWSI_API __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define LIGHTWSI_API __attribute__((visibility("default")))
#else
#	error Unsupport Compiler Detected!
#endif

#define LWSI_VKAPI_CALL(RET) extern "C" LIGHTWSI_API VKAPI_ATTR RET VKAPI_CALL
#define LWSI_LOG_ERROR(...) 
#define LWSI_LOG_WARNING(...)
#define LWSI_LENGTH(a) (sizeof(a) / sizeof(a[0]))

#define LWSI_SWAPPABLE(T) T(T &&other) : T{} { other.Swap(*this); } T &operator=(T &&other) { T(std::move(other)).Swap(*this); return *this; }  T(const T &) = delete; T &operator=(const T &) = delete;
