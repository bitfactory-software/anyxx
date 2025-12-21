#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {
struct x_t {
  std::string s_;
  [[maybe_unused]][[nodiscard]] std::string get() const { return s_; }
};
ANY(has_meta_data, , , )
}  // namespace

TEST_CASE("unerase_cast") {
  using namespace anyxx;
  using namespace anyxx;
  using value = shared_const;

  using value_with_meta_data = has_meta_data<value>;

  {
    auto e = erased<value>(std::make_shared<x_t>("hallo"));
    REQUIRE(unchecked_unerase_cast<x_t>(e)->s_ == "hallo");
  }
  {
    value_with_meta_data v(std::make_shared<x_t>("hallo"));
    try {
      auto x = unerase_cast<x_t>(v)->s_;
      CHECK(x == "hallo");
    } catch (anyxx::type_mismatch_error&) {
      CHECK(false);
    }
    CHECK_THROWS_AS(unerase_cast<std::string>(v), type_mismatch_error);
  }
}
