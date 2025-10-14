#include <catch.hpp>
#include <string>
#include <anypp/any++.hpp>

using namespace anypp;
using namespace anypp;
using namespace anypp;
using namespace anypp;

namespace typed_any_test {
struct x_t {
  std::string s_;
};
}

namespace typed_any_test {
ANY(test_i, (ANY_CONST_METHOD(std::string, to_string),
                      ANY_METHOD(void, from_string, std::string_view)))
}  // namespace typed_any_test

using namespace typed_any_test;

TEST_CASE("typed_any/observer") {
  x_t s{"hallo"};

  test_i<const_observer> co{s};
  static_assert(is_any<test_i<const_observer>>);
  CHECK(unerase_cast<x_t>(co)->s_ == "hallo");
  CHECK_THROWS_AS(unerase_cast<std::string>(co),
                  type_mismatch_error);

  auto co_typed = as<x_t>(co);
  CHECK(co_typed->s_ == "hallo");
  CHECK((*co_typed).s_ == "hallo");
  CHECK_THROWS_AS(as<std::string>(co), type_mismatch_error);

  test_i<mutable_observer> mo{s};
  CHECK(unerase_cast<x_t>(mo)->s_ == "hallo");
  CHECK_THROWS_AS(unerase_cast<std::string>(mo),
                  type_mismatch_error);

  auto mo_typed = as<x_t>(mo);
  CHECK(mo_typed->s_ == "hallo");
  CHECK((*mo_typed).s_ == "hallo");
  (*mo_typed) = x_t{"world"};
  CHECK(mo_typed->s_ == "world");
  CHECK(co_typed->s_ == "world");
  CHECK(s.s_ == "world");
}

namespace typed_any_test {

template <>
struct test_i_v_table_map<x_t> {
  static auto to_string(x_t const* x) { return x->s_; }
  static void from_string(x_t* x, std::string_view s) { x->s_ = s; }
};

}  // namespace typed_any_test

namespace {

 template <typename X>
concept can_call_from_string = requires(X x, std::string_view s) {
  { x.from_string(s) };
};
}  // namespace

TEST_CASE("typed_any/observer/test_i") {
  x_t s{"hallo"};

  test_i<const_observer> co{s};
  static_assert(is_any<test_i<const_observer>>);
  CHECK(unerase_cast<x_t>(co)->s_ == "hallo");
  CHECK_THROWS_AS(unerase_cast<std::string>(co),
                  type_mismatch_error);

  CHECK(co.to_string() == "hallo");

  auto co_typed = as<x_t>(co);
  CHECK(co_typed->s_ == "hallo");
  CHECK((*co_typed).s_ == "hallo");
  CHECK(co_typed.to_string() == "hallo");
  CHECK_THROWS_AS(as<std::string>(co), type_mismatch_error);

  test_i<mutable_observer> mo{s};
  CHECK(unerase_cast<x_t>(mo)->s_ == "hallo");
  CHECK_THROWS_AS(unerase_cast<std::string>(mo),
                  type_mismatch_error);

  CHECK(mo.to_string() == "hallo");

  auto mo_typed = as<x_t>(mo);
  CHECK(mo_typed->s_ == "hallo");
  CHECK((*mo_typed).s_ == "hallo");
  (*mo_typed) = x_t{"world"};
  CHECK(mo_typed->s_ == "world");
  CHECK(co_typed->s_ == "world");
  CHECK(s.s_ == "world");

  mo_typed.from_string("by");
  CHECK(mo_typed->s_ == "by");
  CHECK(co_typed->s_ == "by");
  CHECK(s.s_ == "by");

  static_assert(!can_call_from_string<decltype(co_typed)>);
  static_assert(can_call_from_string<decltype(mo_typed)>);
}
