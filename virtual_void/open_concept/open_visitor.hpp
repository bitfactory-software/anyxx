#pragma once

#include <virtual_void/open_concept/open_concept.hpp>

namespace virtual_void::open_concept {

using visitor_in_param = const void*;
using visitor_out_param = void*;

template <typename INTERFACE_NAME>
struct visitor_methods {
  using interface_name_t = INTERFACE_NAME;
  using model =
      virtual_void::open_concept::model<INTERFACE_NAME,
                                        data::has_no_meta::const_observer>;
  using method_t = extension_method<INTERFACE_NAME,
                                    void(virtual_void::const_, visitor_in_param,
                                         visitor_in_param)>;
  method_t head, center, tail;
};

template <typename INTERFACE_NAME>
using visit =
    extension_method<INTERFACE_NAME,
                     void(virtual_void::const_, visitor_methods<INTERFACE_NAME> const&,
                          visitor_out_param, visitor_in_param const&)>;

template <typename INTERFACE_NAME, typename OUT, typename IN>
class visitor : public visitor_methods<INTERFACE_NAME> {
 public:
  template <typename CLASS>
  using self_t =
      typename visitor_methods<INTERFACE_NAME>::method_t::template class_param_t<CLASS>;
  template <typename CLASS>
  using typed_implementation_function_type = auto (*)(self_t<CLASS>, OUT&,
                                                      IN const&) -> void;

  // private:
  template <typename CLASS>
  auto define_(typename visitor_methods<INTERFACE_NAME>::method_t& method,
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

  template <is_virtual_void ERASED_DATA>
  void operator()(model<INTERFACE_NAME, ERASED_DATA> const& m,
                  visit<INTERFACE_NAME> const& visit, OUT& out,
                  IN const& in = {}) const {
    visit(m, *this, &out, &in);
  }
  template <typename CLASS>
  auto operator()(CLASS const* p, visit<INTERFACE_NAME> const& visit, OUT& out,
                  IN const& in = {}) const {
    return (*this)(model<INTERFACE_NAME, data::has_no_meta::const_observer>{*p},
                   out, in);
  }
};

}  // namespace virtual_void::open_concept