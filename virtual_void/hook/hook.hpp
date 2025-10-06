#pragma once

#include <cassert>
#include <functional>
#include <ranges>
#include <vector>

namespace virtual_void {

template <typename R, typename... ARGS>
class hook;
template <typename R, typename... ARGS>
class hook<R(ARGS...)> {
 public:
  class connection {
    int id_;
    hook* hook_;
    friend class hook;

    connection(int id, hook* hook) : id_(id), hook_(hook) {}
    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

   public:
    connection(connection&&) = default;
    connection& operator=(connection&&) = default;

    ~connection() {
      if (hook_) hook_->remove(std::move(*this));
    }
  };

  class super {
    int index_;
    hook const& hook_;
    friend class hook;

    super(int index, hook const& hook) : index_(index), hook_(hook) {}

   public:
    explicit operator bool() const { return index_ >= 0; }
    R operator()(ARGS&&... args) const {
      assert(index_ >= 0);
      return hook_.callees_[index_].second(super{index_ - 1, hook_},
                             std::forward<ARGS>(args)...);
    }
  };

  using callee = std::function<R(typename super const&, ARGS...)>;

  R operator()(ARGS&&... args) const {
    assert(!callees_.empty());
    return callees_.back().second(super{((int)callees_.size()) - 2, *this},
                                  std::forward<ARGS>(args)...);
  }

  [[nodiscard]]
  connection insert(callee const& f) {
    callees_.emplace_back(entry{next_id_, f});
    return connection{next_id_++, this};
  }

  void remove(connection&& c) {
    std::erase_if(callees_, [&](auto const id_callee_pair) {
      return id_callee_pair.first == c.id_;
    });
    c.hook_ = nullptr;
  }

 private:
  int next_id_ = 0;
  using entry = std::pair<int, callee>;
  std::vector<entry> callees_;
};

}  // namespace virtual_void