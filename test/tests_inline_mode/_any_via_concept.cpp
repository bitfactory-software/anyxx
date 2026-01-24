#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

TRAIT(trait1, (ANY_METHOD(std::string, fn1, (), const)))

}  // namespace

TEST_CASE("any via concept simple") {
  using namespace anyxx;

  struct model1 {
    std::string fn1() const { return "model1"; }
  };
  any<value, trait1> test{model1{}};
  CHECK(test.fn1() == "model1");
}