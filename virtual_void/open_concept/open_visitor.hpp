#pragma once

#include <virtual_void/open_concept/open_concept.hpp>

namespace virtual_void::open_concept {

template <typename TAG>
struct visitor {

  struct in_param {
    const void* p;
  };
  struct out_param {
    void* p;
  };

  using model =
      virtual_void::open_concept::model<TAG,
                                        data::has_no_meta::const_observer>;
  using method_t =
      extension_method<TAG, void(virtual_void::const_, out_param const&,
                                       in_param const&)>;
  method_t head, center, tail;
};


//template <typename VISITOR, typename OUT, typename IN>
//struct typed_visitor 
//{
//  template <typename CLASS, typename FUNCTION>
//  auto define_center(FUNCTION f) {
//      auto wrapped = []
//    auto fp = ensure_function_ptr<CLASS, method_t::class_param_t, R, ARGS...>(f);
//    auto& v_table = v_table_instance<INTERFACE_NAME, CLASS>();
//    remember(remebered_implementations, v_table, fp);
//    return fp;
//  }
//
//};

}  // namespace virtual_void::open_concept