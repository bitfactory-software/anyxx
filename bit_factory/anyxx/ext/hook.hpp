#pragma once

#include <bit_factory/anyxx/errors/error.hpp>
#include <functional>
#include <vector>
#include <algorithm>
#include <cassert>

namespace anyxx {

/// \brief A class template to implement a hook/callback chain.
template <typename R, typename... Args>
class hook;
template <typename R, typename... Args>
class hook<R(Args...)> {
 public:
  struct connection_info {
    int id;
    hook* owner;
  };
  class connection {
    connection_info info_;
    friend class hook;

    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

   public:
    // cppcheck-suppress-begin noExplicitConstructor
    explicit(false) connection(connection_info info) : info_(info) {}
    // cppcheck-suppress-end noExplicitConstructor
    connection& operator=(connection_info info) {
      close();
      info_ = info;
      return *this;
    }
    connection(connection&&) = default;
    connection& operator=(connection&&) = default;
    void close() {
      if (info_.owner) info_.owner->remove(info_.id);
      info_.owner = nullptr;
    }

    ~connection() { close(); }
  };

  class super {
    int index_;
    hook const& hook_;
    friend class hook;

    super(int index, hook const& hook) : index_(index), hook_(hook) {}

   public:
    explicit operator bool() const { return index_ >= 0; }
    R operator()(Args&&... args) const {
      assert(index_ >= 0);
      return hook_.callees_[((std::size_t)index_)].second(
          super{index_ - 1, hook_}, std::forward<Args>(args)...);
    }
  };

  using callee = std::function<R(super const&, Args...)>;

  R operator()(Args&&... args) const {
    assert(!callees_.empty());
    return callees_.back().second(super{((int)callees_.size()) - 2, *this},
                                  std::forward<Args>(args)...);
  }

  connection_info insert(callee const& f) {
    callees_.emplace_back(entry{next_id_, f});
    return connection_info{next_id_++, this};
  }

 private:
  void remove(int id) {
    std::erase_if(callees_, [&](auto const& id_callee_pair) {
      return id_callee_pair.first == id;
    });
  }

  int next_id_ = 0;
  using entry = std::pair<int, callee>;
  std::vector<entry> callees_;
};

}  // namespace anyxx