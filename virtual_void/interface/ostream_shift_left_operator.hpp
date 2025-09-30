#pragma once

#include <ostream>

#undef interface

#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

namespace virtual_void::interface {

VV_INTERFACE(ostreamable, (VV_CONST_METHOD(void, to_ostream, std::ostream&)))

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

template <data::is_erased_data VV>
std::ostream& operator<<(std::ostream& o, ostreamable<VV> const& i) {  // 1
  i.to_ostream(o);
  return o;
}

};  // namespace virtual_void::interface
