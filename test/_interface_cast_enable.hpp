
#include <string>

#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;

namespace test_query_interface {

struct X {
  double d_;
  std::string to_string() const { return std::to_string(d_); }
  double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

ERASED_INTERFACE(to_string_i, (INTERFACE_CONST_METHOD(std::string, to_string)))
ERASED_INTERFACE(get_value_i, (INTERFACE_CONST_METHOD(double, get_value)))
ERASED_INTERFACE_(set_value_i, get_value_i,
                  (INTERFACE_METHOD(void, set_value, double)))

void enable_casts();
}

