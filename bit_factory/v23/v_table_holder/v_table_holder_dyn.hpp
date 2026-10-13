#pragma once

#include <bit_factory/v23/v_table_holder/v_table_holder_forward.hpp>

namespace anyxx {

template <typename Trait>
using v_table_t = typename Trait::v_table_t;

template <typename Trait>
using v_table_deduced_type_t = typename v_table_t<Trait>::deduced_type;

template <typename Proxy, typename Trait>
  requires is_dyn<Proxy>
struct v_table_holder<Proxy, Trait> : v_table_deduced_type_t<Trait> {
 public:
  using v_table_t = Trait::v_table_t;

 private:
  v_table_t* v_table_ = nullptr;

 public:
  v_table_holder() = default;
  explicit v_table_holder(v_table_t* v_table) : v_table_(v_table) {}
  void set_v_table_ptr(v_table_t* v_table) { v_table_ = v_table; }
  // cppcheck-suppress-begin [functionConst, functionStatic]
  auto get_v_table_ptr() const { return v_table_; }
  // cppcheck-suppress-end [functionConst, functionStatic]
  template <typename ProxyImpl, typename Concrete>
  void init_v_table() {
    v_table_ =
        v_table_instance<v_table_t, anyxx::unerased<ProxyImpl, Concrete>>();
  }
  auto release_v_table() { return std::exchange(v_table_, nullptr); }
};

}  // namespace anyxx