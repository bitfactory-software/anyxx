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
struct typed_visitor {
  VISITOR vistor; 
  template <typename CLASS>
  using self_t = typename VISITOR::method_t::template class_param_t<CLASS>;
  template <typename CLASS>
  using typed_implementation_function_type = auto (*)(self_t<CLASS>, OUT&,
                                                      IN const&) -> void;
  template <typename CLASS>
  auto define_center(typed_implementation_function_type<CLASS> f) {
    return vistor.center.template define<CLASS>(f);
  }
};

}  // namespace virtual_void::open_concept