#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>
#include <mutex>
#include <string>

using namespace anyxx;

namespace {

struct widget_a {
  [[nodiscard]] std::string get() const { return "widget_a"; }
};

TRAIT(widget, dynamic_copyable, (FN(std::string, get, (), const)))

any_<weak, widget> any_widget_weak;

void observe(int expected_use_count) {
  auto s = std::make_shared<int>(1);
  std::weak_ptr<int> w = s;

  CHECK(get_proxy(any_widget_weak).use_count() == expected_use_count);
  // we have to make a copy of shared pointer before usage:
  auto any_widget_shared_const = lock<any_<shared, widget>>(any_widget_weak);
  if (expected_use_count > 0)
    CHECK(any_widget_shared_const);
  else
    CHECK(!any_widget_shared_const);
}

}  // namespace

TEST_CASE("example X1/ weak cppreference") {
  // https://en.cppreference.com/w/cpp/memory/weak_ptr.html
  {
    auto any_widget_shared_const =
        any_<shared, widget>{std::make_shared<widget_a>()};
    static_assert(
        borrowable_from<weak, shared, any_<shared, widget>::v_table_t>);
    any_widget_weak = any_widget_shared_const;
    observe(1);
  }
  observe(0);
}

