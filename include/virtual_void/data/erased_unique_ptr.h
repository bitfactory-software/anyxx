#pragma once

#include <memory>

namespace virtual_void::data {
template <typename ERASED>
using data_deleter = void (*)(ERASED*);
template <typename ERASED>
using erased_unique_ptr = std::unique_ptr<ERASED, data_deleter<ERASED>>;
}  // namespace virtual_void::data
