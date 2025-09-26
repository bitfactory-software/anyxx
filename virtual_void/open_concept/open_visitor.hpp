#pragma once

#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/open_concept/open_concept.hpp>
//
//namespace virtual_void::open_concept {
//
//using visitor_in_param = const void*;
//using visitor_out_param = void*;
//
//template <is_extendable_v_table EXTENDED_V_TABLE>
//struct visitor_methods {
//  using method_t = extension_method<is_extendable_v_table,
//                                    void(virtual_void::const_, visitor_in_param,
//                                         visitor_in_param)>;
//  method_t head, center, tail;
//};
//
//template <is_extendable_v_table EXTENDED_V_TABLE>
//using visit =
//    extension_method<EXTENDED_V_TABLE,
//                     void(virtual_void::const_,
//                          visitor_methods<EXTENDED_V_TABLE> const&,
//                          visitor_out_param, visitor_in_param const&)>;
//
//template <is_extendable_v_table EXTENDED_V_TABLE, typename OUT, typename IN>
//class visitor : public visitor_methods<EXTENDED_V_TABLE> {
// public:
//  template <typename CLASS>
//  using self_t = typename visitor_methods<
//      EXTENDED_V_TABLE>::method_t::template class_param_t<CLASS>;
//  template <typename CLASS>
//  using typed_implementation_function_type = auto (*)(self_t<CLASS>, OUT&,
//                                                      IN const&) -> void;
//
//  using const_observer_interface_t =
//      interface_for<EXTENDED_V_TABLE, data::has_no_meta::const_observer>;
//
//  // private:
//  template <typename CLASS>
//  auto define_(typename visitor_methods<INTERFACE_NAME>::method_t& method,
//               typed_implementation_function_type<CLASS> f) {
//    return method.template define<CLASS>(f);
//  }
//
// public:
//  template <typename CLASS>
//  auto define_head(this auto& self,
//                   typed_implementation_function_type<CLASS> f) {
//    return self.define_<CLASS>(self.head, f);
//  }
//  template <typename CLASS>
//  auto define_center(this auto& self,
//                     typed_implementation_function_type<CLASS> f) {
//    return self.define_<CLASS>(self.center, f);
//  }
//  template <typename CLASS>
//  auto define_tail(this auto& self,
//                   typed_implementation_function_type<CLASS> f) {
//    return self.define_<CLASS>(self.tail, f);
//  }
//
//  template <interface::is_interface INTERFACE>
//    requires std::derived_from<typename INTERFACE::v_table_t, EXTENDED_V_TABLE>
//  void operator()(INTERFACE const& m, visit<EXTENDED_V_TABLE> const& visit,
//                  OUT& out, IN const& in = {}) const {
//    visit(m, *this, &out, &in);
//  }
//  template <typename CLASS>
//  auto operator()(CLASS const* p, visit<EXTENDED_V_TABLE> const& visit,
//                  OUT& out, IN const& in = {}) const {
//    return (*this)(const_observer_interface_t{*p}, out, in);
//  }
//};
//
//}  // namespace virtual_void::open_concept