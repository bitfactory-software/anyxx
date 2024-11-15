#pragma once

#include <optional>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include "../virtual_void.h"

namespace virtual_void::interface {

template <typename VOID>
struct v_table_base {
  using void_t = VOID;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_base) == from;
  }
  bool (*_is_derived_from)(const std::type_info&);
  template <typename UNERASE>
  v_table_base(UNERASE)
      : _is_derived_from([](const std::type_info& from) {
          return static_is_derived_from(from);
        }){};
};

template <is_virtual_void VIRTUAL_VOID>
class base {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = typename virtual_void_trait<VIRTUAL_VOID>::void_t;
  using v_table_t = v_table_base<void_t>;
  template <typename>
  using is_already_base =
      std::false_type;  // base is always at the bottom of the v_table chain.
 protected:
  virtual_void_t virtual_void_ = nullptr;
  v_table_t* v_table_ = nullptr;

 public:
  base(virtual_void_t virtual_void, v_table_t* v_table)
      : virtual_void_(std::move(virtual_void)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  base(CONSTRUCTED_WITH&& constructed_with)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>,
                                base<VIRTUAL_VOID>> &&
             !is_virtual_void<std::remove_cvref_t<CONSTRUCTED_WITH>> &&
             !is_virtual_typed<std::remove_cvref_t<CONSTRUCTED_WITH>>)
      : virtual_void_(erased<virtual_void_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    static v_table_t imlpemented_v_table{
        unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_v_table;
    static_assert(!is_virtual_typed<CONSTRUCTED_WITH>);
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
  base(const base&) = default;
  // base(base&) requires(std::is_copy_constructible_v<base>) = default;
  base(base&&) = default;

  template< is_virtual_void OTHER>
  friend class base;

  template <is_virtual_void VV>
  friend auto& get_virtual_void(base<VV> const& interface) {
    return interface.virtual_void_;
  }
  template <is_virtual_void VV>
  friend auto& get_v_table(base<VV> const& interface) {
    return interface.v_table_;
  }

  void operator()() const {}
  void* operator[](void*) const {}

 protected:
  base() = default;
};

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

template <typename TO, typename FROM>
TO static_v_table_cast(const FROM& from)
  requires(std::derived_from<TO, FROM>)
{
  return *static_cast<const TO*>(&from);
}
template <typename TO, typename FROM>
std::optional<TO> v_table_cast(const FROM& from)
  requires(std::derived_from<TO, FROM>)
{
  if (is_derived_from<TO>(from)) return {*static_cast<const TO*>(&from)};
  return {};
}

template <typename ERASED_TO>
auto pure_v_table_cast(const auto& v_table) {
  return static_cast<typename ERASED_TO::v_table_t*>(v_table);
}

template <typename TO, typename FROM>
TO interface_lifetime_cast(const FROM& from) {
  return TO(lifetime_cast<typename TO::virtual_void_t>(*from),
            pure_v_table_cast<TO>(from.get_v_table()));
}

}  // namespace virtual_void::interface
