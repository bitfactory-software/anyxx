#pragma once

#include <memory>

namespace virtual_void::data {
template <typename ERASED>
using data_deleter = void (*)(ERASED*);
template <typename ERASED>
using erased_unique_ptr = std::unique_ptr<ERASED, data_deleter<ERASED>>;

template <typename ERASED_TO, typename FROM>
erased_unique_ptr<ERASED_TO> erase_unique_ptr(FROM from) {
  return erased_unique_ptr<ERASED_TO>{ from.release(), from.geet_deleter() };
}
}  // namespace virtual_void::data
