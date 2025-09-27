#pragma once

#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/interface/extension_method.hpp>

namespace virtual_void::interface {

using visitor_in_param = const void*;
using visitor_out_param = void*;

template <interface::is_interface INTERFACE>
struct visitor_methods {
  using method_t =
      extension_method<INTERFACE, void(virtual_void::const_, visitor_in_param,
                                       visitor_in_param)>;
  method_t head, center, tail;
};

template <interface::is_interface INTERFACE>
using visit = extension_method<
    INTERFACE, void(virtual_void::const_, visitor_methods<INTERFACE> const&,
                    visitor_out_param, visitor_in_param const&)>;

template <interface::is_interface INTERFACE, typename OUT, typename IN>
class visitor : public visitor_methods<INTERFACE> {
 public:
  using extended_v_table_t = typename INTERFACE::v_table_t;
  using visitor_method_t = typename visitor_methods<INTERFACE>::method_t;

  template <typename CLASS>
  using self_t = visitor_method_t::template class_param_t<CLASS>;

  template <typename CLASS>
  using typed_implementation_function_type = auto (*)(self_t<CLASS>, OUT&,
                                                      IN const&) -> void;

  using observer_interface_t =
      typename visitor_method_t::observer_interface_t;

 private:
  template <typename CLASS>
  auto define_(visitor_method_t& method,
               typed_implementation_function_type<CLASS> f) {
    return method.template define<CLASS>(f);
  }

 public:
  template <typename CLASS>
  auto define_head(this auto& self,
                   typed_implementation_function_type<CLASS> f) {
    return self.define_<CLASS>(self.head, f);
  }
  template <typename CLASS>
  auto define_center(this auto& self,
                     typed_implementation_function_type<CLASS> f) {
    return self.define_<CLASS>(self.center, f);
  }
  template <typename CLASS>
  auto define_tail(this auto& self,
                   typed_implementation_function_type<CLASS> f) {
    return self.define_<CLASS>(self.tail, f);
  }

  void operator()(observer_interface_t const& m, visit<INTERFACE> const& visit,
                  OUT& out, IN const& in = {}) const {
    visit(m, *this, &out, &in);
  }
  template <typename CLASS>
  auto operator()(CLASS const* p, visit<INTERFACE> const& visit,
                  OUT& out, IN const& in = {}) const {
    return (*this)(observer_interface_t{*p}, out, in);
  }
};

}  // namespace 