#include <iostream>

#include "include/catch.hpp"

// import virtual_void;
#include "virtual_void/data/has_type_info/observer.h"
#include "virtual_void/data/has_type_info/shared_const.h"
#include "virtual_void/interface/base.h"
#include "virtual_void/interface/declare_macro.h"
#include "virtual_void/interface/registry.h"

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data::has_type_info;

namespace {
struct X {
  double d_;
  std::string to_string() const { return std::to_string(d_); }
  double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

ERASED_INTERFACE(to_string_i, (INTERFACE_CONST_METHOD(std::string, to_string)))
ERASED_INTERFACE(get_value_i, (INTERFACE_CONST_METHOD(double, get_value)))
ERASED_INTERFACE(set_value_i, (INTERFACE_METHOD(void, set_value, double)))

}  // namespace

TEST_CASE("prototype") {
  enable_const_observer_copy<get_value_i, X, meta>();
//      const_observer_copy<X, meta>);
  const_observer_copy<X, meta>.seal_for_runtime();
  const_observer_copies<meta>.seal_for_runtime();
  //  get_value_i<const_observer> get_value_i_co =
  //  query_interface<get_value_i<const_observer>>(registry);

  // virtual_void::interface::cast query_interface;
  // enable<X, to_string_i>( query_interface );
  // enable<X, get_value_i>( query_interface );
  // enable<X, set_value>( query_interface );

  X x{3.14};
  to_string_i<const_observer> to_string_i_co{x};
  REQUIRE(to_string_i_co.to_string() == "3.140000");
  auto base =
      const_observer_copy<X, meta>.construct<get_value_i<const_observer>>(
          get_virtual_void(to_string_i_co));
  auto i = interface::static_v_table_cast<get_value_i<const_observer>>(base);
  REQUIRE(i.get_value() == 3.14);
  std::cout << "prototype: " << i.get_value() << "\n";

  get_value_i<const_observer> i1 =
      query_interface<get_value_i<const_observer>>(to_string_i_co);
  REQUIRE(i1.get_value() == 3.14);
  std::cout << "prototype i1: " << i1.get_value() << "\n";
}
