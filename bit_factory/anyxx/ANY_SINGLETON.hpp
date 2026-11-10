#pragma once

#include <bit_factory/anyxx/ANY_UNIQUE_NAME.hpp>

/// \addtogroup singleton_macros ANY_SINGLETON macros
/// \brief Macros to reduce boilerplate for declaring/defining static
/// runtime data.
///
/// To enable some dynamic functionality (crosscast, open dispatch)
/// there must be some static data holding information to perform
/// these operations. Extra care must be taken in a DLL scenario.
///
/// To reduce boilerplate code, Any++ supplies these macros.
///  @{

/// \brief Declare access to a singleton object in a header file.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param name Name of the singleton.
/// \param __VA_ARGS__ Type of the singleton object.
///
/// This macro should reside inside the namespace you wish the
/// singleton to reside. See also \ref ANY_SINGLETON.
#define ANY_SINGLETON_DECLARE(export_, name, ...) \
  using name##_t = __VA_ARGS__;                   \
  export_ extern name##_t& get_##name();          \
  static inline name##_t& name = get_##name();

/// \brief Define a singleton object in a source file.
/// \param namespace_ The namespace for the singleton.
/// \param name Name of the singleton.
/// \param __VA_ARGS__ Parameters for the singleton constructor.
///
/// See also \ref ANY_SINGLETON_DECLARE.
#define ANY_SINGLETON(namespace_, name, ...)       \
  namespace_::name##_t& namespace_::get_##name() { \
    static name##_t singleton_{__VA_ARGS__};       \
    return singleton_;                             \
  };
///  @}

