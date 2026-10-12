#pragma once

namespace anyxx {

#ifdef ANY_DLL_MODE
constexpr bool is_in_dll_mode = true;
#else
constexpr bool is_in_dll_mode = false;
#endif

}  // namespace anyxx