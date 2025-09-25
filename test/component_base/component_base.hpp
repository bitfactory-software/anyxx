#include <string>
#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;

#ifndef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT
#endif

#ifdef COMPONENT_BASE_SHARED
#ifdef _MSVC_LANG
#undef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT __declspec(dllexport)
#pragma message("component_base -> dll")
#endif
#endif

namespace test::component_base {

template <typename T>
struct to_string_i_default_v_table_map {
  auto to_string(T const* x) -> std::string { return x->to_string(); };
};
template <typename T>
struct to_string_i_v_table_map : to_string_i_default_v_table_map<typename T> {};
struct to_string_i_v_table : ::virtual_void::interface::base_v_table {
  using v_table_base_t = ::virtual_void::interface::base_v_table;
  using v_table_t = to_string_i_v_table;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : v_table_base_t::static_is_derived_from(from);
  }
  std::string (*to_string)(void const*);
  ;
  template <typename CONCRETE>
  to_string_i_v_table(std::in_place_type_t<CONCRETE> concrete)
      : v_table_base_t(concrete) {
    using v_table_map = to_string_i_v_table_map<CONCRETE>;
    to_string = [](void const* _vp) -> std::string {
      return v_table_map{}.to_string(unchecked_unerase_cast<CONCRETE>(_vp));
    };
    ;
    ::virtual_void::interface::set_is_derived_from<v_table_t>(this);
  };
  template <typename CONCRETE>
  static auto imlpementation() {
    static to_string_i_v_table v_table{std::in_place_type<CONCRETE>};
    return &v_table;
  }
};
template <typename VIRTUAL_VOID>
struct to_string_i : ::virtual_void::interface::base<VIRTUAL_VOID> {
  using virtual_void_t = VIRTUAL_VOID;
  using base_t = ::virtual_void::interface::base<VIRTUAL_VOID>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t = to_string_i_v_table;
  template <typename CONCRETE>
  static auto v_table_imlpementation() {
    return v_table_t::template imlpementation<CONCRETE>();
  }
  using base_t::v_table_;
  using base_t::virtual_void_;
  to_string_i(virtual_void_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  to_string_i(CONSTRUCTED_WITH&& v)
    requires virtual_void::interface::constructibile_for<CONSTRUCTED_WITH,
                                                         VIRTUAL_VOID>
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    v_table_ =
        v_table_imlpementation<unerased_type<VIRTUAL_VOID, CONSTRUCTED_WITH>>();
  }
  template <typename CONSTRUCTED_WITH>
  to_string_i(
      const virtual_void::virtual_typed<CONSTRUCTED_WITH, virtual_void_t>& vt)
      : to_string_i(*vt) {}
  template <typename OTHER>
  to_string_i(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  template <typename OTHER>
  to_string_i(const OTHER&& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(std::move(other)) {}
  std::string to_string() const
    requires(::virtual_void::const_correct_for_virtual_void<void const*,
                                                            virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->to_string(
        virtual_void::get_data(base_t::virtual_void_));
  }
  auto const* operator->() const { return this; }
  to_string_i() = default;
  to_string_i(to_string_i const&) = default;
  to_string_i(to_string_i&&) = default;
  to_string_i& operator=(to_string_i const&) = default;
  to_string_i& operator=(to_string_i&&) = default;
  template <virtual_void::is_virtual_void OTHER>
  friend class virtual_void::interface::base;
  template <typename TO, typename FROM>
  friend TO virtual_void::interface::unchecked_v_table_cast(FROM from)
    requires(std::derived_from<TO, FROM>);
};
VV_INTERFACE(get_value_i, (VV_CONST_METHOD(double, get_value)))
VV_INTERFACE_(set_value_i, get_value_i, (VV_METHOD(void, set_value, double)))

COMPONENT_BASE_EXPORT
to_string_i<virtual_void::data::has_meta_runtime::const_observer>
get_to_string_i_co();
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_meta_runtime::shared_const>
get_to_string_i_sc(double v);
COMPONENT_BASE_EXPORT to_string_i<virtual_void::data::has_meta_runtime::unique>
get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_meta_runtime::shared_const sc_X(
    double v);
COMPONENT_BASE_EXPORT virtual_void::data::has_meta_runtime::unique u_X(double v);

}  // namespace test::component_base
