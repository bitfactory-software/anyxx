#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "test/component_base/component_base.hpp"

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;
using namespace test::component_base;

static_assert(anyxx::is_in_dll_mode);

namespace {

ANY(any_test, (ANY_METHOD(int, fun, (), const)), const_observer, dyn)

struct test_class {
  [[nodiscard]] int fun() const { return 42; }
};

anyxx::factory<any_test, std::string> make_any_test;
auto ___ = make_any_test.register_("test_class", []() { return test_class{}; });

}  // namespace

TEST_CASE("_dyns_v_table_in_dll_mode") {
  auto a = make_any_test.construct<shared_const>("test_class");
  CHECK(a.fun() == 42);
}
