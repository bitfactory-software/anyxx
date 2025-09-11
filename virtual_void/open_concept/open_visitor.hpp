#pragma once

#include <virtual_void/open_concept/open_concept.hpp>

namespace virtual_void::open_concept {

template <typename INTERFACE>
struct visitor {

  struct in_param {
    const void* p;
  };
  struct out_param {
    void* p;
  };

  using model =
      virtual_void::open_concept::model<INTERFACE,
                                        data::has_no_meta::const_observer>;
  using method_t =
      extension_method<INTERFACE, void(virtual_void::const_, out_param const&,
                                       in_param const&)>;
  method_t head, center, tail;
};

}  // namespace virtual_void::open_concept