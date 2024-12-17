#pragma once

#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <virtual_void/utillities/VV_EXPORT.hpp>
#include <virtual_void/virtual_void.hpp>

#undef interface

namespace virtual_void::interface {

struct base_v_table {
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(base_v_table) == from;
  }
  bool (*_is_derived_from)(const std::type_info&);
  base_v_table(auto unused)
      : _is_derived_from([](const std::type_info& from) {
          return static_is_derived_from(from);
        }){};
};

VV_EXPORT base_v_table* base_v_table_imlpementation();
VV_EXPORT int next_i_table_index_value();

template <typename V_TABLE>
int i_table_index_implemntation() {
  static int i = next_i_table_index_value();
  return i;
}
template <typename V_TABLE>
int i_table_index();

template <is_virtual_void VIRTUAL_VOID>
class base;

template <typename CONSTRUCTED_WITH, typename VIRTUAL_VOID>
concept constructibile_for =
    !std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>,
                       base<VIRTUAL_VOID>> &&
    !is_virtual_void<std::remove_cvref_t<CONSTRUCTED_WITH>> &&
    !is_virtual_typed<std::remove_cvref_t<CONSTRUCTED_WITH>> &&
    (!std::is_const_v<std::remove_reference_t<CONSTRUCTED_WITH>> ||
     virtual_void_trait<VIRTUAL_VOID>::is_constructibile_from_const);

template <is_virtual_void VIRTUAL_VOID>
class base {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = typename virtual_void_trait<VIRTUAL_VOID>::void_t;
  using v_table_t = base_v_table;

 protected:
  virtual_void_t virtual_void_ = {};
  v_table_t* v_table_ = nullptr;

 public:
  base() = default;
  base(virtual_void_t virtual_void, v_table_t* v_table)
      : virtual_void_(std::move(virtual_void)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  base(CONSTRUCTED_WITH&& constructed_with)
    requires constructibile_for<CONSTRUCTED_WITH, VIRTUAL_VOID>
      : virtual_void_(erased<virtual_void_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    v_table_ = base_v_table_imlpementation();
  }
  template <typename CONSTRUCTED_WITH>
  base(const virtual_typed<CONSTRUCTED_WITH, virtual_void_t>& vt) : base(*vt) {}
  template <typename OTHER>
  base(const OTHER& other)
    requires(std::derived_from<OTHER, base<VIRTUAL_VOID>>)
      : virtual_void_(get_virtual_void(other)), v_table_(get_v_table(other)) {}
  template <typename OTHER>
  base(OTHER&& other)
    requires(std::derived_from<OTHER, base<VIRTUAL_VOID>>)
      : virtual_void_(std::move(other.virtual_void_)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  base& operator=(OTHER&& other)
    requires(std::derived_from<OTHER, base<VIRTUAL_VOID>>)
  {
    virtual_void_ = std::move(other.virtual_void_);
    v_table_ = get_v_table(other);
    return *this;
  }
  base(const base&) = default;
  // base(base&) requires(std::is_copy_constructible_v<base>) = default;
  base(base&& rhs) noexcept
      : virtual_void_(std::move(rhs.virtual_void_)), v_table_(rhs.v_table_) {}
  base& operator=(base const& other) = default;

  template <is_virtual_void OTHER>
  friend class base;

  template <is_virtual_void VIRTUAL_VOID>
  friend inline auto& get_virtual_void(base<VIRTUAL_VOID> const& interface);
  template <is_virtual_void VIRTUAL_VOID>
  friend inline auto move_virtual_void(base<VIRTUAL_VOID>&& interface);
  template <is_virtual_void VIRTUAL_VOID>
  friend inline auto get_interface_data(base<VIRTUAL_VOID> const& interface);
  template <is_virtual_void VIRTUAL_VOID>
  friend inline auto& get_v_table(base<VIRTUAL_VOID> const& interface);

  template <typename TO, typename FROM>
  friend inline TO unchecked_v_table_cast(FROM from)
    requires(std::derived_from<TO, FROM>);

  void operator()() const {}
  void* operator[](void*) const { return {}; }
  explicit operator bool() const {
    return static_cast<bool>(get_virtual_void(*this));
  }
};

template <is_virtual_void VIRTUAL_VOID>
auto& get_virtual_void(base<VIRTUAL_VOID> const& interface) {
  return interface.virtual_void_;
}
template <is_virtual_void VIRTUAL_VOID>
auto move_virtual_void(base<VIRTUAL_VOID>&& interface) {
  return std::move(interface.virtual_void_);
}
template <is_virtual_void VIRTUAL_VOID>
auto get_interface_data(base<VIRTUAL_VOID> const& interface) {
  return get_data(get_virtual_void(interface));
}
template <is_virtual_void VIRTUAL_VOID>
inline auto& get_v_table(base<VIRTUAL_VOID> const& interface) {
  return interface.v_table_;
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
  return TO{std::move(from.virtual_void_),
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
  return TO(lifetime_cast<typename TO::virtual_void_t>(*from),
            pure_v_table_cast<TO>(from.get_v_table()));
}

}  // namespace virtual_void::interface
