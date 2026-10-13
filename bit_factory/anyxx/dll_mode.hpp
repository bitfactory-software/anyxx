#pragma once

namespace anyxx {

#ifdef ANY_DLL_MODE
constexpr bool is_in_dll_mode = true;
#else
constexpr bool is_in_dll_mode = false;
#endif

/// \defgroup anyxx_config Any++ configuration macro
/// \brief Macro to configure Any++ for DLL mode
///
/// If ANY_DLL_MODE is #defined, Any++ is configured for DLL mode. In
/// DLL mode, some static runtime data is not instantiated in the
/// header implicitly via static inline and must be manually
/// instantiated in a single translation unit.
///
/// See also \ref ANY_SINGLETON_DECLARE, \ref ANY_SINGLETON, \ref
/// ANY_META_CLASS_FWD, \ref ANY_META_CLASS, \ref
/// ANY_DISPATCH_COUNT_FWD, \ref ANY_DISPATCH_COUNT, \ref
/// ANY_DISPATCH_FOR_FWD, \ref ANY_DISPATCH_FOR, \ref
/// ANY_META_CLASS_FWD, \ref ANY_META_CLASS.

}  // namespace anyxx