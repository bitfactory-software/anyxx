#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace _2p {

TRAIT(nullable,
      (ANY_FN_DEF(public, bool, has_value, (), const, [x]() { return true; }),
       ANY_FN_DEF(public, T, get_value, (), const, [x]() { return x; })))

}

TEST_CASE("test optional") {
  using namespace anyxx;
  using namespace _2p;
   
  using_<int>::as<nullable> optional_int{42};
  CHECK(optional_int.has_value());
  CHECK(optional_int.get_value() == 42);
}