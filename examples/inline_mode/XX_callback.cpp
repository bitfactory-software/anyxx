#include <catch.hpp>
#include <coroutine>
#include <print>

using namespace Catch::Matchers;

namespace co_callback {
template <typename R>
[[noreturn]]
R rethrow_exception(std::exception_ptr exception) {
  std::rethrow_exception(exception);
  return R{};
}
template <typename R = nullptr_t>
struct [[nodiscard]] callback {
  struct promise_type {
    callback<R> get_return_object() {
      return callback<R>{
          std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    struct resume_callback {
      resume_callback() noexcept {}
      bool await_ready() const noexcept { return false; }
      void await_suspend(
          std::coroutine_handle<promise_type> thisCoroutine) noexcept {
        auto& promise = thisCoroutine.promise();
        if (promise.callingCoroutine_) promise.callingCoroutine_.resume();
      }
      void await_resume() noexcept {}
    };
    auto initial_suspend() noexcept { return std::suspend_never{}; }
    auto final_suspend() noexcept { return resume_callback{}; }
    void return_value(R result) { result_ = result; }
    void unhandled_exception() noexcept {
      exception_ = std::current_exception();
    }
    R result_ = {};
    std::coroutine_handle<> callingCoroutine_ = {};
    std::exception_ptr exception_ = {};
  };

  callback(const callback&) = delete;
  callback& operator=(const callback&) = delete;
  callback& operator=(callback&& r) noexcept = delete;

  callback() noexcept = default;
  callback(callback&& t) noexcept { std::swap(coroutine_, t.coroutine_); }
  explicit callback(std::coroutine_handle<promise_type> coroutine)
      : coroutine_(coroutine) {}
  ~callback() noexcept {
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
using api_callback = std::function<void(R)>;
template <typename R>
using api = std::function<void(api_callback<R>)>;
template <typename R>
struct callback_awaiter {
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
template <typename R>
auto co_callback(api<R> api) {
  return callback_awaiter<R>{api};
}

}  // namespace co_callback

TEST_CASE("example XX/ callback1") {
  using namespace co_callback;

  int step = 1;

  auto do_with_42 = [&](std::function<void(int)> cb) {
    std::println("before callback");
    CHECK(step++ == 1);
    cb(42);
    std::println("after callback");
    CHECK(step++ == 3);
  };

  auto test1 = [&] -> callback<int> {
    auto _42 = co_await co_callback::co_callback<int>(do_with_42);
    std::println("recieving 42");
    CHECK(step++ == 2);
    CHECK(42 == _42);
    co_return _42;
  };
  CHECK(test1().get_result() == 42);

  CHECK(step == 4);
}
