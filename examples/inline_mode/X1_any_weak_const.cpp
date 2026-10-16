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

TRAIT(widget, dynamic_copyable, (ANY_FN(std::string, get, (), const)))

any<weak, widget> any_widget_weak;

void observe(int expected_use_count) {
  auto s = std::make_shared<int>(1);
  std::weak_ptr<int> w = s;

  CHECK(get_proxy(any_widget_weak).use_count() == expected_use_count);
  // we have to make a copy of shared pointer before usage:
  auto any_widget_shared_const = lock<any<shared, widget>>(any_widget_weak);
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
        any<shared, widget>{std::make_shared<widget_a>()};
    static_assert(
        borrowable_from<weak, shared, any<shared, widget>::v_table_t>);
    any_widget_weak = any_widget_shared_const;
    observe(1);
  }
  observe(0);
}

namespace {
any<shared, widget> load_widget([[maybe_unused]] int id) {
  return std::make_shared<widget_a>();
}

static std::map<int, any<weak, widget>> cache;  // out of function for CHECK
static std::mutex cache_mutex;
any<shared, widget> make_widget(int id) {
  std::lock_guard hold{cache_mutex};
  return *lock<any<shared, widget>>(cache[id]).or_else([&] -> std::optional<any<shared, widget>> {
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
    CHECK(get_proxy(w1).use_count() == 1);
    CHECK(cache.size() == 1);
    CHECK(get_proxy(cache[0]).use_count() == 1);
    {
      auto w2 = make_widget(0);
      CHECK(w2.get() == "widget_a");
      CHECK(get_proxy(w2).use_count() == 2);
      CHECK(cache.size() == 1);
      CHECK(get_proxy(cache[0]).use_count() == 2);
    }
    CHECK(get_proxy(w1).use_count() == 1);
    CHECK(cache.size() == 1);
    CHECK(get_proxy(cache[0]).use_count() == 1);
  }
  CHECK(get_proxy(cache[0]).use_count() == 0);
}