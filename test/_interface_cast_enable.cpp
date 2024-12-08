#include "_interface_cast_enable.hpp"

#include <virtual_void/data/has_type_info/observer.hpp>
#include <virtual_void/data/has_type_info/shared_const.hpp>
#include <virtual_void/data/has_type_info/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/cast.hpp>
#include <virtual_void/interface/declare_macro.hpp>

#include <catch.hpp>

void test_query_interface::enable_casts() {
  using namespace virtual_void;
  using namespace virtual_void::interface;
  using namespace virtual_void::data::has_type_info;

  enable_copy_cast<get_value_i, X, const_observer>();
  enable_copy_cast<get_value_i, X, mutable_observer>();
  enable_copy_cast<get_value_i, X, shared_const>();
  enable_copy_cast<get_value_i, X, unique>();
  enable_copy_cast<get_value_i, X, unique, shared_const>();
  enable_copy_cast<set_value_i, X, unique, shared_const>();
  enable_copy_cast<to_string_i, X, const_observer, unique>();
  enable_copy_cast<set_value_i, X, mutable_observer, unique>();

  enable_move_cast<to_string_i, X, unique, unique>();
  enable_move_cast<get_value_i, X, shared_const, unique>();
  REQUIRE(find_move<shared_const, unique>.is_defined<X>());
  REQUIRE(
      move<X, shared_const, unique>.is_defined<get_value_i<shared_const>>());
  static_assert(std::same_as<decltype(move<X, shared_const, unique>)::result_t,
                             base<shared_const>>);
  seal_casts();

  move_factory_method<shared_const, unique> const& move_unique_to_shared =
      find_move<shared_const, unique>.construct<X>();
}
