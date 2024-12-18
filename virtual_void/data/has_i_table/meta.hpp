#pragma once

#include <virtual_void/utillities/VIRTUAL_DESTRUCTOR_FOR_DEBUGGING.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {
class i_table;

template <typename CLASS>
constexpr i_table* get_i_table_of();

constexpr const std::type_info& get_type_info(i_table const*);
}

namespace virtual_void::data::has_i_table {

template <class META>
concept is_i_table_meta = is_meta<META> && requires(META meta) {
  { meta.get_i_table() };
};

struct meta {
  virtual_void::meta::i_table* i_table_ = nullptr;

  template <typename T>
  constexpr meta(std::in_place_type_t<T>)
      : i_table_(virtual_void::meta::get_i_table_of<std::decay_t<T>>()) {}
  template <is_i_table_meta META>
  constexpr meta(const META& rhs) : i_table_(rhs.i_table_) {}

  constexpr meta() noexcept = default;
  constexpr meta(const meta&) = default;
  constexpr meta(meta&) = default;
  constexpr meta(meta&& rhs) noexcept { swap(*this, rhs); }
  constexpr meta& operator=(meta const&) = default;
  constexpr meta& operator=(meta&& rhs) noexcept {
    meta destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend constexpr void swap(meta& lhs, meta& rhs) noexcept {
    using namespace std;
    swap(lhs.i_table_, rhs.i_table_);
  }

  constexpr const auto* get_meta() const { return this; }

  constexpr type_info_ptr type_info() const {
    return &virtual_void::meta::get_type_info(get_i_table());
  }

  constexpr virtual_void::meta::i_table* get_i_table() const { return i_table_; }

  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING ~meta() = default;
};

}  // namespace virtual_void::data::has_i_table