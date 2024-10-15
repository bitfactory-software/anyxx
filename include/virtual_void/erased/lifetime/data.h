#pragma once

#include <stdexcept>
#include <type_traits>

#include "../concept.h"

namespace virtual_void::erased {

struct empty_t {};

template <typename MORE = empty_t>
struct abstract_data_t : MORE {
  void* data_ = nullptr;
  abstract_data_t(void* data) : data_(data) {}
#ifdef DEBUG
  virtual ~abstract_data_t() = default;
#endif  // DEBUG
};

template <typename M, typename MORE = empty_t>
struct concrete_data_t : abstract_data_t<MORE> {
  M m_;
  concrete_data_t(M&& m)
      : abstract_data_t<MORE>(std::addressof(m_)), m_(std::move(m)) {}
  template <typename... ARGS>
  concrete_data_t(std::in_place_t, ARGS&&... args)
      : abstract_data_t<MORE>(std::addressof(m_)),
        m_(std::forward<ARGS>(args)...) {}
};

using abstract_data = abstract_data_t<empty_t>;
template <typename M>
using concrete_data = concrete_data_t<M, empty_t>;

template <typename U, typename MORE = empty_t>
U* reconcrete_cast(abstract_data_t<MORE>& a) {
  return static_cast<U*>(a.data_);
}
template <typename U, typename MORE = empty_t>
const U* reconcrete_cast(const abstract_data_t<MORE>& a) {
  return static_cast<const U*>(a.data_);
}

}  // namespace virtual_void::erased