#pragma once

#include <virtual_void/interface/i_table.hpp>
#include <virtual_void/utillities/VIRTUAL_DESTRUCTOR_FOR_DEBUGGING.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data::has_i_table {

template <class META>
concept is_i_table_meta = is_meta<META> && requires(META meta) {
  {
    meta.get_const_i_table()
  } -> std::convertible_to<interface::i_table<const_>*>;
  {
    meta.get_mutable_i_table()
  } -> std::convertible_to<interface::i_table<mutable_>*>;
  {
    meta.template get_i_table<const_>()
  } -> std::convertible_to<interface::i_table<const_>*>;
  {
    meta.template get_i_table<mutable_>()
  } -> std::convertible_to<interface::i_table<mutable_>*>;
};

struct meta {
  interface::i_table<const_>* const_i_table_ = nullptr;
  interface::i_table<mutable_>* mutable_i_table_ = nullptr;

  template <typename T>
  constexpr meta(std::in_place_type_t<T>)
      : const_i_table_(interface::i_table_of<std::decay_t<T>, const_>()),
        mutable_i_table_(interface::i_table_of<std::decay_t<T>, mutable_>()) {}
  template <is_i_table_meta META>
  constexpr meta(const META& rhs)
      : const_i_table_(rhs.const_i_table_),
        mutable_i_table_(rhs.mutable_i_table_) {}

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
    swap(lhs.const_i_table_, rhs.const_i_table_);
    swap(lhs.mutable_i_table_, rhs.mutable_i_table_);
  }

  constexpr const auto* get_meta() const { return this; }

  constexpr type_info_ptr type_info() const { return &get_const_i_table()->type(); }

  constexpr interface::i_table<const_>* get_const_i_table() const {
    return const_i_table_;
  }
  constexpr interface::i_table<mutable_>* get_mutable_i_table() const {
    return mutable_i_table_;
  }

  template <is_constness CONSTNESS>
  interface::i_table<CONSTNESS>* get_i_table() const {
    if constexpr (std::same_as<CONSTNESS, const_>) {
      return get_const_i_table();
    } else {
      get_mutable_i_table();
    }
  }

  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING ~meta() = default;
};

}  // namespace virtual_void::data::has_i_table