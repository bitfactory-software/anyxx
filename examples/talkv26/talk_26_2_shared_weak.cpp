#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>

namespace {

struct widget_1 {
  [[nodiscard]] std::string get() const { return "widget_1"; }
};
struct widget_2 {
  [[nodiscard]] std::string get() const { return "widget_2"; }
};

using namespace anyxx;
template <is_trait Trait, typename Self>
struct widget : deleteable<Trait, Self> {
  std::string get() const;
  using default_proxy_t = shared;
};

any<widget> load_widget([[maybe_unused]] int id) {
  if (id == 0) {
    return {std::in_place_type<widget_1>};
  } else {
    return {std::in_place_type<widget_2>};
  }
}

static std::map<int, any_<weak, widget>> cache;  // out of function for CHECK
static std::mutex cache_mutex;
any<widget> make_widget(int id) {
  std::lock_guard __{cache_mutex};
  return *lock<any<widget>>(cache[id]).or_else(
      [&] -> std::optional<any<widget>> {
      auto s = load_widget(id);
    cache[id] = s;
    return s;
  });
}

TEST_CASE("talk_26_2_shared_weak sutters favorite 10 cpp lines") {
  // https://youtu.be/JfmTagWcqoE?si=sKPgf2sU-3q6p0TU&t=2027
  {
    auto w1 = make_widget(0);
    CHECK(w1.get() == "widget_1");
    CHECK(get_proxy(w1).use_count() == 1);
    CHECK(cache.size() == 1);
    CHECK(get_proxy(cache[0]).use_count() == 1);
    {
      auto w2 = make_widget(0);
      CHECK(w2.get() == "widget_1");
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

}  // namespace
