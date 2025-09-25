#pragma once

#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/meta/base_v_table.hpp>
#include <virtual_void/utillities/VV_EXPORT.hpp>
#include <virtual_void/virtual_void.hpp>

#undef interface

namespace virtual_void::interface {

using base_v_table = meta::base_v_table;

template <is_virtual_void ERASED_DATA>
class base;

template <typename CONSTRUCTED_WITH, typename ERASED_DATA>
concept constructibile_for =
    erased_constructibile_for<CONSTRUCTED_WITH, ERASED_DATA,
                              base<ERASED_DATA>>;

template <is_virtual_void ERASED_DATA>
class base {
 public:
  using erased_data_t = ERASED_DATA;
  using void_t = typename erased_data_trait<ERASED_DATA>::void_t;
  using v_table_t = meta::base_v_table;

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
      : erased_data_(erased<erased_data_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    using t = unerased_type<ERASED_DATA, CONSTRUCTED_WITH>;
    v_table_ = meta::base_v_table_imlpementation<t>();
  }
  template <typename CONSTRUCTED_WITH>
  base(const virtual_typed<CONSTRUCTED_WITH, erased_data_t>& vt) : base(*vt) {}
  template <typename OTHER>
  base(const OTHER& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t>)
      : erased_data_(data::copy_convert_to<ERASED_DATA>(other.erased_data_)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  base(OTHER&& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t>)
      : erased_data_(data::move_convert_to<ERASED_DATA>(
            std::move(other.erased_data_))),
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

  template <is_virtual_void OTHER>
  friend class base;

  template <is_virtual_void ERASED_DATA>
  friend inline auto& get_virtual_void(base<ERASED_DATA> const& interface);
  template <is_virtual_void ERASED_DATA>
  friend inline auto move_virtual_void(base<ERASED_DATA>&& interface);
  template <is_virtual_void ERASED_DATA>
  friend inline auto get_interface_data(base<ERASED_DATA> const& interface);
  template <is_virtual_void ERASED_DATA>
  friend inline auto& get_v_table(base<ERASED_DATA> const& interface);

  template <typename TO, typename FROM>
  friend inline TO unchecked_v_table_cast(FROM from)
    requires(std::derived_from<TO, FROM>);

  void operator()() const {}
  void* operator[](void*) const { return {}; }
  explicit operator bool() const {
    return static_cast<bool>(get_virtual_void(*this));
  }
};

template <is_virtual_void ERASED_DATA>
auto& get_virtual_void(base<ERASED_DATA> const& interface) {
  return interface.erased_data_;
}
template <is_virtual_void ERASED_DATA>
auto move_virtual_void(base<ERASED_DATA>&& interface) {
  return std::move(interface.erased_data_);
}
template <is_virtual_void ERASED_DATA>
auto get_interface_data(base<ERASED_DATA> const& interface) {
  return get_data(get_virtual_void(interface));
}
template <is_virtual_void ERASED_DATA>
inline auto& get_v_table(base<ERASED_DATA> const& interface) {
  return interface.v_table_;
}

inline bool is_derived_from(const std::type_info& from,
                            meta::base_v_table const* base_v_table) {
  return base_v_table->_is_derived_from(from);
}
template <is_virtual_void VV>
bool is_derived_from(const std::type_info& from, base<VV> const& interface) {
  return get_v_table(interface)->_is_derived_from(from);
}
template <typename FROM, is_virtual_void VV>
bool is_derived_from(base<VV> const& interface) {
  return is_derived_from(typeid(FROM::v_table_t), interface);
}

template <typename V_TABLE>
void set_is_derived_from(auto v_table) {
  v_table->_is_derived_from = +[](const std::type_info& from) {
    return V_TABLE::static_is_derived_from(from);
  };
}

template <typename TO>
auto pure_v_table_cast(const auto& v_table) {
  return static_cast<typename TO::v_table_t*>(v_table);
}

template <typename TO, typename FROM>
TO unchecked_v_table_cast(FROM from)
  requires(std::derived_from<TO, FROM>)
{
  return TO{std::move(from.erased_data_),
            pure_v_table_cast<TO>(from.v_table_)};
}

template <typename TO, typename FROM>
std::optional<TO> v_table_cast(const FROM& from)
  requires(std::derived_from<TO, FROM>)
{
  if (is_derived_from<TO>(from)) return {unchecked_v_table_cast<TO>(from)};
  return {};
}

template <typename TO, typename FROM>
TO interface_lifetime_cast(const FROM& from) {
  return TO(lifetime_cast<typename TO::erased_data_t>(*from),
            pure_v_table_cast<TO>(from.get_v_table()));
}

}  // namespace virtual_void::interface
