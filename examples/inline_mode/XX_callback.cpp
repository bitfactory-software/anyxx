#include <anyxx/anyxx.hpp>
#include <catch.hpp>
#include <coro_callback/coro_callback.hpp>
#include <coroutine>
#include <print>

using namespace Catch::Matchers;

TEST_CASE("example XX/ recieve1") {
  int step = 1;

  // + lib callback style
  auto int_callback_api = ([&](std::function<void(int)> recieve) {
    std::println("before recieve");
    CHECK(step++ == 1);
    recieve(42);
    std::println("after recieve");
    CHECK(step++ == 3);
  });
  // - lib callback style

  // + app callback style
  step = 1;
  CHECK(step == 1);
  int_callback_api([&](int _42) {
    std::println("recieving 42");
    CHECK(step++ == 2);
    CHECK(42 == _42);
  });
  CHECK(step == 4);
  // - app callback style

  // + lib wrapped for coro style
  auto int_recieve_coro = [&]() {
    return coro_callback::wrap<int>(int_callback_api);
  };
  // - lib wrapped for coro style
  step = 1;
  CHECK(step == 1);
  [&] -> coro_callback::recieve<void> {
    // + app coro style must exist inside acoro
    auto _42 = co_await int_recieve_coro();
    std::println("recieving 42");
    CHECK(step++ == 2);
    CHECK(42 == _42);
    // - app coro style
  }();
  CHECK(step == 4);
}

TEST_CASE("example XX/ recieve2") {
  int step = 1;

  // + lib callback style
  auto void_callback_api = [&](std::function<void(void)> recieve) {
    std::println("before recieve");
    CHECK(step++ == 1);
    recieve();
    std::println("after recieve");
    CHECK(step++ == 3);
  };
  // - lib callback style

  // + app callback style
  step = 1;
  CHECK(step == 1);
  void_callback_api([&] {
    std::println("recieving");
    CHECK(step++ == 2);
  });
  CHECK(step == 4);
  // - app callback style

  // + lib wrapped for coro style
  auto void_recieve_coro = [&]() {
    return coro_callback::wrap<void>(void_callback_api);
  };
  // - lib wrapped for coro style
  step = 1;
  [&] -> coro_callback::recieve<void> {
    // + app coro style must exist inside acoro
    co_await void_recieve_coro();
    std::println("recieving");
    CHECK(step++ == 2);
    // - app coro style
  }();
  CHECK(step == 4);
}
