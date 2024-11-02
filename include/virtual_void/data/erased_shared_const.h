#pragma once

#include "erased_shared_const_ptr.h"

namespace virtual_void::data {

template <typename BASE>
using erased_shared_const = erased_shared_const_ptr<BASE>;

template <typename BASE, typename T, typename... ARGS>
erased_shared_const<BASE> make_erased_shared_const(ARGS&&... args) {
  return std::make_shared<T const>(std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data