#pragma once

#include <ostream>

#include "base.h"
#include "declare_macro.h"

namespace virtual_void::interface {

ERASED_INTERFACE(ostreamable,
                 (INTERFACE_CONST_METHOD(void, to_ostream, std::ostream&)))

template <typename T>
concept is_ostreamable = requires(T const& t, std::ostream o) {
  { o << t } -> std::same_as<std::ostream&>;
};

static_assert(is_ostreamable<double>);

template <typename OSTREAMABLE>
  requires is_ostreamable<OSTREAMABLE>
struct ostreamable_v_table_map<OSTREAMABLE>
    : ostreamable_default_v_table_map<OSTREAMABLE> {
  void to_ostream(OSTREAMABLE const* x, std::ostream& o) { o << (*x); };
};

template <virtual_void::is_virtual_void VV>
std::ostream& operator<<(std::ostream& o, ostreamable<VV> const& i) {  // 1
  i.to_ostream(o);
  return o;
}

};  // namespace virtual_void::interface
