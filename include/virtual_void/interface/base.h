#pragma once

#include <optional>
#include <type_traits>
#include <typeinfo>

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
  using void_t = typename virtual_void_trait<VIRTUAL_VOID>::void_t;                                    \
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
                                base<VIRTUAL_VOID>>)
      : virtual_void_(std::forward<CONSTRUCTED_WITH>(constructed_with)) {
    static v_table_t imlpemented_v_table{
        unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_v_table;
  }
  template <typename OTHER>
  base(const OTHER& other)
    requires(std::derived_from<OTHER, base<VIRTUAL_VOID>>)
      : virtual_void_(*other), v_table_(other.get_v_table()) {}
  base(const base&) = default;
  base(base&) = default;
  base(base&&) = default;
  virtual_void_t const& operator*() const { return virtual_void_; }
  virtual_void_t& operator*() { return virtual_void_; }
  v_table_t* get_v_table() const { return v_table_; }
  bool is_derived_from(const std::type_info& from) const {
    return v_table_->_is_derived_from(from);
  }
  template <typename FROM>
  bool is_derived_from() const {
    return is_derived_from(typeid(FROM::v_table_t));
  }

 protected:
  base() = default;
};

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
  if (from.is_derived_from<TO>()) return {*static_cast<const TO*>(&from)};
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


}  // namespace virtual_void

