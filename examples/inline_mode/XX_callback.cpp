#include <catch.hpp>
#include <coroutine>
#include <print>

using namespace Catch::Matchers;

namespace coro_callback {
template <typename R>
[[noreturn]]
R rethrow_exception(std::exception_ptr exception) {
  std::rethrow_exception(exception);
  return R{};
}
template <typename R>
struct recieve {
  template <typename Promise, typename HandleReturn>
  struct basic_promise_type : HandleReturn {
    recieve<R> get_return_object(this auto& self) {
      return recieve<R>{std::coroutine_handle<Promise>::from_promise(self)};
    }

    struct resume_recieve {
      resume_recieve() noexcept {}
      bool await_ready() const noexcept { return false; }
      void await_suspend(
          std::coroutine_handle<Promise> thisCoroutine) noexcept {
        auto& promise = thisCoroutine.promise();
        if (promise.callingCoroutine_) promise.callingCoroutine_.resume();
      }
      void await_resume() noexcept {}
    };
    auto initial_suspend() noexcept { return std::suspend_never{}; }
    auto final_suspend() noexcept { return resume_recieve{}; }
    void unhandled_exception() noexcept {
      exception_ = std::current_exception();
    }
    std::coroutine_handle<> callingCoroutine_ = {};
    std::exception_ptr exception_ = {};
  };
  template <typename R>
  struct handle_return {
    void return_value(R result) { result_ = result; }
    R result_ = {};
  };
  template <>
  struct handle_return<void> {
    void return_void() {};
  };
  struct promise_type : basic_promise_type<promise_type, handle_return<R>> {};

  recieve(const recieve&) = delete;
  recieve& operator=(const recieve&) = delete;
  recieve& operator=(recieve&& r) noexcept = delete;

  recieve() noexcept = default;
  recieve(recieve&& t) noexcept { std::swap(coroutine_, t.coroutine_); }
  explicit recieve(std::coroutine_handle<promise_type> coroutine)
      : coroutine_(coroutine) {}
  ~recieve() noexcept {
    if (coroutine_) coroutine_.destroy();
  }

  bool await_ready() const noexcept { return false; }
  void await_suspend(auto callingCoroutine) noexcept {
    callingCoroutine.resume();
  }
  auto handle_resume(auto handle_exception) {
    if (!coroutine_) return R{};
    if (auto exception = coroutine_.promise().exception_)
      return handle_exception(exception);
    return coroutine_.promise().result_;
  }
  auto await_resume() { return handle_resume(rethrow_exception<R>); }
  auto get_result(auto handle_exception) {
    return handle_resume(handle_exception);
  }
  auto get_result() { return get_result(rethrow_exception<R>); }

  std::coroutine_handle<promise_type> coroutine_;
};

template <typename R>
using api_recieve = std::function<void(R)>;
template <typename R>
using api = std::function<void(api_recieve<R>)>;
template <typename R>
struct recieve_awaiter {
  bool await_ready() { return false; }
  void await_suspend(auto callingContinuation) {
    bool called = false;
    api_([this, callingContinuation, called](const R& r) mutable {
      if (called) return;
      called = true;
      result_ = r;
      callingContinuation.resume();
    });
  }
  R await_resume() { return result_; }

  const api<R> api_;
  R result_ = {};
};
template <>
struct recieve_awaiter<void> {
  bool await_ready() { return false; }
  void await_suspend(auto callingContinuation) {
    bool called = false;
    api_([this, callingContinuation, called]() mutable {
      if (called) return;
      called = true;
      callingContinuation.resume();
    });
  }
  void await_resume() {}
  const api<void> api_;
};
template <typename R>
auto wrap(api<R> api) {
  return recieve_awaiter<R>{api};
}

}  // namespace coro_callback

TEST_CASE("example XX/ recieve1") {
  int step = 1;

 auto int_callback_api = ([&](std::function<void(int)> recieve) {
      std::println("before recieve");
      CHECK(step++ == 1);
      recieve(42);
      std::println("after recieve");
      CHECK(step++ == 3);
    });

  step = 1;
  CHECK(step == 1);
  int_callback_api([&](int _42) {
    std::println("recieving 42");
    CHECK(step++ == 2);
    CHECK(42 == _42);
  });
  CHECK(step == 4);

  auto int_recieve_coro = [&]() {
    return coro_callback::wrap<int>(int_callback_api);
  };
  step = 1;
  auto test1 = [&] -> coro_callback::recieve<int> {
    auto _42 = co_await int_recieve_coro();
    std::println("recieving 42");
    CHECK(step++ == 2);
    CHECK(42 == _42);
    co_return _42;
  };
  CHECK(step == 1);
  CHECK(test1().get_result() == 42);
  CHECK(step == 4);
}

TEST_CASE("example XX/ recieve2") {
  int step = 1;

  auto void_callback_api = [&](std::function<void(void)> recieve) {
    std::println("before recieve");
    CHECK(step++ == 1);
    recieve();
    std::println("after recieve");
    CHECK(step++ == 3);
  };

  step = 1;
  CHECK(step == 1);
  void_callback_api([&] {
    std::println("recieving");
    CHECK(step++ == 2);
  });
  CHECK(step == 4);

  auto void_recieve_coro = [&]() {
    return coro_callback::wrap<void>(void_callback_api);
  };
  step = 1;
  auto test2coro = [&] -> coro_callback::recieve<void> {
    co_await void_recieve_coro();
    std::println("recieving");
    CHECK(step++ == 2);
  };
  CHECK(step == 1);
  test2coro();
  CHECK(step == 4);
}
