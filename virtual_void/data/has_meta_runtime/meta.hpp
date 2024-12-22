#pragma once

#include <virtual_void/meta/forward.hpp>
#include <virtual_void/utillities/VIRTUAL_DESTRUCTOR_FOR_DEBUGGING.hpp>
#include <virtual_void/utillities/VV_EXPORT.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data::has_meta_runtime {

template <class META>
concept is_meta_runtime = is_meta<META> && requires(META meta) {
  { meta.get_type_info() } -> std::convertible_to<std::type_info const&>;
};

struct meta {
  virtual_void::meta::type_info* type_info_ = nullptr;

  const auto* get_meta() const { return this; }

  template <typename T>
  meta(std::in_place_type_t<T>)
      : type_info_(&virtual_void::meta::runtime<virtual_void::meta::type_info,
                                                std::decay_t<T>>()) {}
  template <is_meta_runtime META>
  meta(const META& rhs) : type_info_(rhs.type_info_) {}

  meta() noexcept = default;
  meta(const meta&) = default;
  meta(meta&) = default;
  meta(meta&& rhs) noexcept { swap(*this, rhs); }
  meta& operator=(meta const&) = default;
  meta& operator=(meta&& rhs) noexcept {
    meta destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(meta& lhs, meta& rhs) noexcept {
    using namespace std;
    swap(lhs.type_info_, rhs.type_info_);
  }

  auto type_info() const { return type_info_; }
  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING ~meta() = default;
};

VV_EXPORT std::type_info const* get_std_type_info(meta const& m);

}  // namespace virtual_void::data::has_meta_runtime
