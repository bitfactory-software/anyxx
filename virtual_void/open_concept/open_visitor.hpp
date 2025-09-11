#pragma once

#include <virtual_void/open_concept/open_concept.hpp>

namespace virtual_void::open_concept {

template <typename TAG>
struct visitor {
  using in_param = const void*;
  using out_param = void*;

  using model =
      virtual_void::open_concept::model<TAG, data::has_no_meta::const_observer>;
  using method_t =
      extension_method<TAG, void(virtual_void::const_, out_param, in_param)>;
  method_t head, center, tail;
};

template <typename VISITOR, typename OUT, typename IN>
class typed_visitor {
 public:
  template <typename CLASS>
  using self_t = typename VISITOR::method_t::template class_param_t<CLASS>;
  template <typename CLASS>
  using typed_implementation_function_type = auto (*)(self_t<CLASS>, OUT&,
                                                      IN const&) -> void;

// private:
  VISITOR vistor;
  template <typename CLASS>
  auto define(typename VISITOR::method_t& method,
              typed_implementation_function_type<CLASS> f) {
    return method.template define<CLASS>(f);
  }

public:
    template <typename CLASS>
  auto define_head(typed_implementation_function_type<CLASS> f) {
    return define<CLASS>(vistor.head, f);
  }
  template <typename CLASS>
  auto define_center(typed_implementation_function_type<CLASS> f) {
    return define<CLASS>(vistor.center, f);
  }
  template <typename CLASS>
  auto define_tail(typed_implementation_function_type<CLASS> f) {
    return define<CLASS>(vistor.tail, f);
  }
};

}  // namespace virtual_void::open_concept