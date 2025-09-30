#pragma once

#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/data/trait.hpp>
#include <virtual_void/runtime/meta_data.hpp>

#undef interface

namespace virtual_void::interface {

using base_v_table = runtime::base_v_table; // for declare_macro.hpp
template <data::is_erased_data ERASED_DATA>
class base;

template <typename I>
concept is_interface_impl = requires(I i, I::erased_data_t ed) {
  typename I::void_t;
  typename I::erased_data_t;
  typename I::v_table_t;
  typename I::trait_t;
  { get_virtual_void(i) };
};

template <typename I>
concept is_interface = is_interface_impl<std::decay_t<I>>;

template <class E>
concept is_virtual_typed = is_interface<E> && requires(E e) {
  typename E::trait_t;
  typename E::value_t;
  { E::is_const } -> std::convertible_to<bool>;
};

template <typename CONSTRUCTED_WITH, typename ERASED_DATA>
concept constructibile_for =
    data::erased_constructibile_for<CONSTRUCTED_WITH, ERASED_DATA,
                              base<ERASED_DATA>> &&
    !is_interface<CONSTRUCTED_WITH> &&
    !is_virtual_typed<std::remove_cvref_t<CONSTRUCTED_WITH>>;

template <data::is_erased_data ERASED_DATA>
class base {
 public:
  using erased_data_t = ERASED_DATA;
  using trait_t = data::erased_data_trait<erased_data_t>;
  using void_t = typename trait_t::void_t;
  using v_table_t = runtime::base_v_table;

 protected:
  erased_data_t erased_data_ = {};
  v_table_t* v_table_ = nullptr;

 public:
  base() = default;
  base(erased_data_t virtual_void, v_table_t* v_table)
      : erased_data_(std::move(virtual_void)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  base(CONSTRUCTED_WITH&& constructed_with)
    requires constructibile_for<CONSTRUCTED_WITH, ERASED_DATA>
      : erased_data_(data::erased<erased_data_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    using t = data::unerased_type<ERASED_DATA, CONSTRUCTED_WITH>;
    v_table_ = runtime::base_v_table_imlpementation<t>();
  }
  template <typename OTHER>
  base(const OTHER& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t>)
      : erased_data_(data::copy_convert_to<ERASED_DATA>(
            other.erased_data_, *get_v_table(other)->type_info)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  base(OTHER&& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t>)
      : erased_data_(
            data::move_convert_to<ERASED_DATA>(std::move(other.erased_data_))),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  base& operator=(OTHER&& other)
    requires(std::derived_from<OTHER::v_table_t, v_table_t>)
  {
    erased_data_ =
        data::move_convert_to<ERASED_DATA>(std::move(other.erased_data_));
    v_table_ = get_v_table(other);
    return *this;
  }
  base(const base&) = default;
  // base(base&) requires(std::is_copy_constructible_v<base>) = default;
  base(base&& rhs) noexcept
      : erased_data_(std::move(rhs.erased_data_)), v_table_(rhs.v_table_) {}
  base& operator=(base const& other) = default;

  template <data::is_erased_data OTHER>
  friend class base;

  template <data::is_erased_data ERASED_DATA>
  friend inline auto& get_virtual_void(base<ERASED_DATA> const& interface);
  template <data::is_erased_data ERASED_DATA>
  friend inline auto move_virtual_void(base<ERASED_DATA>&& interface);
  template <data::is_erased_data ERASED_DATA>
  friend inline auto get_interface_data(base<ERASED_DATA> const& interface);
  template <is_interface I>
  friend inline auto get_v_table(I const& interface);

  template <typename TO, typename FROM>
  friend inline TO unchecked_v_table_cast(FROM from)
    requires(std::derived_from<TO, FROM>);

  void operator()() const {}
  void* operator[](void*) const { return {}; }
  explicit operator bool() const {
    return static_cast<bool>(get_virtual_void(*this));
  }
};

template <typename V_TABLE, data::is_erased_data ERASED_DATA>
struct interface_t;

template <typename V_TABLE, data::is_erased_data ERASED_DATA>
using interface_for = typename interface_t<V_TABLE, ERASED_DATA>::type;

template <data::is_erased_data ERASED_DATA>
auto& get_virtual_void(base<ERASED_DATA> const& interface) {
  return interface.erased_data_;
}
template <data::is_erased_data ERASED_DATA>
auto move_virtual_void(base<ERASED_DATA>&& interface) {
  return std::move(interface.erased_data_);
}
template <data::is_erased_data ERASED_DATA>
auto get_interface_data(base<ERASED_DATA> const& interface) {
  return data::get_data(get_virtual_void(interface));
}

template <typename TO>
auto pure_v_table_cast(runtime::base_v_table* v_table) {
  return static_cast<TO*>(v_table);
}
template <is_interface TO>
auto pure_v_table_cast(runtime::base_v_table* v_table) {
  return pure_v_table_cast<typename TO::v_table_t>(v_table);
}

template <is_interface INTERFACE>
inline auto get_v_table(INTERFACE const& interface) {
  return pure_v_table_cast<INTERFACE>(interface.v_table_);
}

template <is_interface INTERFACE>
inline const auto& get_runtime(INTERFACE const& interface) {
  return *get_v_table(interface)->type_info;
}

template <data::is_erased_data VV>
bool is_derived_from(const std::type_info& from, base<VV> const& interface) {
  return get_v_table(interface)->_is_derived_from(from);
}
template <typename FROM, data::is_erased_data VV>
bool is_derived_from(base<VV> const& interface) {
  return is_derived_from(typeid(FROM::v_table_t), interface);
}

template <typename V_TABLE>
void set_is_derived_from(auto v_table) {
  v_table->_is_derived_from = +[](const std::type_info& from) {
    return V_TABLE::static_is_derived_from(from);
  };
}

template <typename TO, typename FROM>
TO unchecked_v_table_cast(FROM from)
  requires(std::derived_from<TO, FROM>)
{
  return TO{std::move(from.erased_data_), pure_v_table_cast<TO>(from.v_table_)};
}

template <typename TO, typename FROM>
std::optional<TO> v_table_cast(const FROM& from)
  requires(std::derived_from<TO, FROM>)
{
  if (is_derived_from<TO>(from)) return {unchecked_v_table_cast<TO>(from)};
  return {};
}

template <typename U, is_interface INTERFACE>
auto unchecked_unerase_cast(INTERFACE const& o) {
  return data::unchecked_unerase_cast<U>(get_virtual_void(o));
}
template <typename U, is_interface INTERFACE>
auto unerase_cast(INTERFACE const& o) {
  return data::unerase_cast<U>(get_virtual_void(o), get_runtime(o));
}
template <typename U, is_interface INTERFACE>
auto unerase_cast(INTERFACE const* o) {
  data::unerase_cast<U>(get_virtual_void(*o), get_runtime(o));
  return nullptr;
}

}  // namespace virtual_void::interface
