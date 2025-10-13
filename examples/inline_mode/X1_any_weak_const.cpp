#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <anypp/any_meta_class.hpp>
#include <anypp/lock.hpp>
#include <anypp/shared_const.hpp>

using namespace Catch::Matchers;

using namespace anypp;

namespace {

struct widget_a {
  std::string get() const { return "widget_a"; }
};

VV_ANY(any_widget, (VV_CONST_METHOD(std::string, get)))

any_widget<weak> any_widget_weak;

void observe(int expected_use_count) {
  auto s = std::make_shared<int>(1);
  std::weak_ptr<int> w = std::move(s);

  CHECK(get_erased_data(any_widget_weak).use_count() == expected_use_count);
  // we have to make a copy of shared pointer before usage:
  auto any_widget_shared_const = lock(any_widget_weak);
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
        any_widget<shared_const>{std::make_shared<widget_a>()};
    static_assert(borrowable_from<weak, shared_const>);
    any_widget_weak = any_widget_shared_const;
    observe(1);
  }
  observe(0);
}

namespace {
any_widget<shared_const> load_widget(int id) {
  return std::make_shared<widget_a>();
}

static std::map<int, any_widget<weak>> cache;  // out of function for CHECK
static std::mutex cache_mutex;
any_widget<shared_const> make_widget(int id) {
  std::lock_guard hold{cache_mutex};
  return *lock(cache[id]).or_else(
      [&] -> std::optional<any_widget<shared_const>> {
        auto s = load_widget(id);
        cache[id] = s;
        return s;
      });
}
}  // namespace

TEST_CASE("example X1/ sutters favorite 10 cpp lines") {
  // https://youtu.be/JfmTagWcqoE?si=sKPgf2sU-3q6p0TU&t=2027
  {
    auto w1 = make_widget(0);
    CHECK(w1.get() == "widget_a");
    CHECK(get_erased_data(w1).use_count() == 1);
    CHECK(cache.size() == 1);
    CHECK(get_erased_data(cache[0]).use_count() == 1);
    {
      auto w2 = make_widget(0);
      CHECK(w2.get() == "widget_a");
      CHECK(get_erased_data(w2).use_count() == 2);
      CHECK(cache.size() == 1);
      CHECK(get_erased_data(cache[0]).use_count() == 2);
    }
    CHECK(get_erased_data(w1).use_count() == 1);
    CHECK(cache.size() == 1);
    CHECK(get_erased_data(cache[0]).use_count() == 1);
  }
  CHECK(get_erased_data(cache[0]).use_count() == 0);
}