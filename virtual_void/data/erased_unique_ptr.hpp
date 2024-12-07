#pragma once

#include <functional>
#include <memory>

namespace virtual_void::data {
template <typename ERASED>
using default_erased_unique_ptr_deleter_t = void (*)(ERASED*);
template <typename ERASED,
          typename DELETER = default_erased_unique_ptr_deleter_t<ERASED>>
using erased_unique_ptr = std::unique_ptr<ERASED, DELETER>;

using void_deleter = std::function<void(void*)>;
using void_erased_unique_ptr = erased_unique_ptr<void, void_deleter>;

template <typename FROM>
auto void_erase_unique_ptr(FROM from) {
  auto original_deleter = from.get_deleter();
  original_deleter(nullptr);
  auto deleter = [original_deleter](void* o) {
    original_deleter(static_cast<FROM::element_type*>(o));
  };
  return void_erased_unique_ptr{static_cast<void*>(from.release()), deleter};
}
}  // namespace virtual_void::data
