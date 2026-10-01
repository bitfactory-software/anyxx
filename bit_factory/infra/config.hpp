#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <expected>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wcast-function-type-mismatch"
#pragma GCC diagnostic ignored "-Wmicrosoft-template-shadow"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wextra-semi"
#endif
#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

#if defined(_MSC_VER)  // MSVC
#define ANYXX_USE_EBO __declspec(empty_bases)
#else
#define ANYXX_USE_EBO
#endif
#if defined(_MSC_VER) && not defined(__clang__)  // MSVC
#define LIFETIMEBOUND [[msvc::lifetimebound]]
#ifndef ANYXX_GODBOLT
#include <CppCoreCheck/Warnings.h>
#pragma warning(default : CPPCORECHECK_LIFETIME_WARNINGS)  // Enable
                                                           // lifetimebound
#else
#endif
#elif defined(__clang__)  // Clang
#define LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define LIFETIMEBOUND
#endif

#if defined(_MSC_VER) && not defined(__clang__)  // MSVC
#define AYXFORCEDINLINE __forceinline
#pragma warning(disable : 4714)
#else
#define AYXFORCEDINLINE inline __attribute__((always_inline))
#endif

