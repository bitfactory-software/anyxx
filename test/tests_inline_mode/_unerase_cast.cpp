#include <catch.hpp>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/any_meta_function.hpp>
#include <virtual_void/meta_data.hpp>

using namespace Catch::Matchers;

namespace {
struct x_t {
  std::string s_;
  std::string get() const { return s_; }
};
VV_ANY(has_meta_data,)
}  // namespace


TEST_CASE("unerase_cast") {
  using namespace virtual_void;
  using namespace virtual_void;
  using value = shared_const;

  using value_with_meta_data = has_meta_data<value>;

  {
    auto e = erased<value>(std::make_shared<x_t>("hallo"));
    REQUIRE(unchecked_unerase_cast<x_t>(e)->s_ == "hallo");
  }
  {
    value_with_meta_data v(std::make_shared<x_t>("hallo"));
    REQUIRE(unerase_cast<x_t>(v)->s_ == "hallo");
    CHECK_THROWS_AS(unerase_cast<std::string>(v), type_mismatch_error);
  }
}