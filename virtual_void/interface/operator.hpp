#pragma once

#undef interface

#include <virtual_void/interface/base.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::interface {

template <typename TARGET, typename BASE_V_TABLE, is_constness CONSTNESS,
          typename RET, typename... ARGS>
struct operator_v_table : BASE_V_TABLE {
  using v_table_base_t = BASE_V_TABLE;
  using v_table_t = operator_v_table;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : BASE_V_TABLE::static_is_derived_from(from);
  }
  RET (*op)(virtual_void::void_t<CONSTNESS>, ARGS&&...);
  template <typename UNERASED>
  operator_v_table(std::in_place_type_t<UNERASED> u) : BASE_V_TABLE(u) {
    op = [](virtual_void::void_t<CONSTNESS> _vp, ARGS&&... args) -> RET {
      return TARGET{}(unchecked_unerase_cast<UNERASED>(_vp), std::forward<ARGS>(args)...);
    };
    set_is_derived_from<v_table_t>(this);
  }
};

template <typename UNERASED, typename TARGET, typename BASE_V_TABLE,
          is_constness CONSTNESS, typename RET, typename... ARGS>
operator_v_table<TARGET, BASE_V_TABLE, CONSTNESS, RET, ARGS...>*
implemented_operator_v_table() {
  static operator_v_table<TARGET, BASE_V_TABLE, CONSTNESS, RET, ARGS...>
      v_table{std::in_place_type<UNERASED>};
  return &v_table;
}

template <typename TARGET, is_erased_data ERASED_DATA,
          template <typename> typename BASE, is_constness CONSTNESS,
          typename RET, typename... ARGS>
struct operator_;
template <typename TARGET, is_erased_data ERASED_DATA,
          template <typename> typename BASE, is_constness CONSTNESS,
          typename RET, typename... ARGS>
struct operator_<TARGET, ERASED_DATA, BASE, CONSTNESS, RET(ARGS...)>
    : BASE<ERASED_DATA> {
 public:
  using erased_data_t = ERASED_DATA;
  using void_t = typename erased_data_trait<ERASED_DATA>::void_t;
  using base_t = BASE<ERASED_DATA>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t =
      operator_v_table<TARGET, v_table_base_t, CONSTNESS, RET, ARGS...>;
  using base_t::operator();

 public:
  using base_t::v_table_;
  using base_t::erased_data_;

 public:
  operator_(erased_data_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  operator_(CONSTRUCTED_WITH&& v)
    requires constructibile_for<CONSTRUCTED_WITH, ERASED_DATA>
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    v_table_ =
        implemented_operator_v_table<unerased_type<ERASED_DATA, CONSTRUCTED_WITH>,
                                     TARGET, v_table_base_t, CONSTNESS, RET,
                                     ARGS...>();
  }
  template <typename OTHER>
  operator_(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  // RET operator()(ARGS&&... args) const
  //   requires(const_correct_for_virtual_void<
  //            virtual_void::void_t<CONSTNESS>, erased_data_t>)
  //{
  //   return static_cast<v_table_t*>(v_table_)->call_op(
  //       get_data(base_t::erased_data_), std::forward<ARGS>(args)...);
  // }
  RET invoke(ARGS&&... args) const
    requires(const_correct_for_virtual_void<virtual_void::void_t<CONSTNESS>,
                                            erased_data_t>)
  {
    return static_cast<v_table_t*>(v_table_)->op(
        get_data(base_t::erased_data_), std::forward<ARGS>(args)...);
  }
  operator_(const operator_&) = default;
  operator_(operator_&) = default;
  operator_(operator_&&) = default;

 protected:
  operator_() = default;
};

};  // namespace virtual_void::interface
