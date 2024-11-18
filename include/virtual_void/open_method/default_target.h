#pragma once

#include "../virtual_void.h"

namespace virtual_void::open_method {

template <typename TARGET = void (*)()>
class default_target {
 public:
  using dispatch_target_t = TARGET;

 private:
  dispatch_target_t default_ =
      reinterpret_cast<dispatch_target_t>(&throw_not_found);

 public:
  static void throw_not_found() { throw error("no target specified."); }
  void define_default(auto f) {
    default_ = reinterpret_cast<dispatch_target_t>(f);
  }
  auto get_default() const { return default_; }
};

}  // namespace virtual_void::open_method