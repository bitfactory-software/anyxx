// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>

#ifdef O

using std::cout;
using std::string;

using namespace anyxx;

namespace anyxx {
template <typename ErasedData>
struct any_inline_base {
  ErasedData erased_data_;
};
}  // namespace anyxx

#define ANY_INLINE_META_FUNCTION(tpl1, tpl2, tpl3, tpl4, tpl, n, BASE, btpl,  \
                                 l)                                           \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                          \
  struct n;                                                                   \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                          \
  struct n##_default_concept_map {                                            \
    _detail_ANYXX_MAP_FUNCTIONS(l)                                            \
  };                                                                          \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                          \
  struct n##_concept_map                                                      \
      : n##_default_concept_map<_detail_ANYXX_TEMPLATE_ARGS(tpl2)> {};        \
                                                                              \
  _detail_ANYXX_V_TABLE_TEMPLATE_HEADER(tpl) struct n##_v_table;              \
                                                                              \
  _detail_ANYXX_V_TABLE_TEMPLATE_HEADER(tpl) struct n##_v_table {             \
    using v_table_t = n##_v_table;                                            \
                                                                              \
    _detail_ANYXX_V_TABLE_FUNCTION_PTRS(l);                                   \
                                                                              \
    template <typename Concrete>                                              \
    explicit(false) n##_v_table(std::in_place_type_t<Concrete>) {             \
      using concept_map = n##_concept_map<_detail_ANYXX_TEMPLATE_ARGS(tpl3)>; \
                                                                              \
      _detail_ANYXX_V_TABLE_LAMBDAS(l);                                       \
    }                                                                         \
  };                                                                          \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                          \
  struct n : BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)> {            \
    using erased_data_t = ErasedData;                                         \
    using base_t = BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)>;       \
    using v_table_t =                                                         \
        n##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(tpl);          \
                                                                              \
    using base_t::erased_data_;                                               \
    v_table_t v_table_;                                                       \
                                                                              \
    template <typename ConstructedWith>                                       \
    explicit(false) n(ConstructedWith&& v)                                    \
      requires anyxx::constructibile_for<ConstructedWith, ErasedData>         \
        : base_t(std::forward<ConstructedWith>(v)),                           \
          v_table_(std::in_place_type<                                        \
                   anyxx::unerased<ErasedData, ConstructedWith>>) {}          \
    template <typename V>                                                     \
    n(std::in_place_t, V&& v)                                                 \
        : base_t(std::in_place, std::forward<V>(v)),                          \
          v_table_(std::in_place_type<anyxx::unerased<ErasedData, V>>) {}     \
    template <typename T, typename... Args>                                   \
    explicit(false) n(std::in_place_type_t<T>, Args&&... args)                \
        : base_t(std::in_place_type<T>, std::forward<Args>(args)...),         \
          v_table_(std::in_place_type<anyxx::unerased<ErasedData, T>>) {}     \
    template <typename Other>                                                 \
    explicit(false) n(const Other& other)                                     \
      requires(std::derived_from<typename Other::v_table_t, v_table_t> &&     \
               anyxx::borrowable_from<erased_data_t,                          \
                                      typename Other::erased_data_t>)         \
        : base_t(other),                                                      \
          v_table_(static_cast<v_table_t const&>(other.v_table_)) {}          \
    template <anyxx::is_any Other>                                            \
    explicit(false) n(Other&& other) noexcept                                 \
      requires(std::derived_from<typename Other::v_table_t, v_table_t> &&     \
               anyxx::moveable_from<erased_data_t,                            \
                                    typename Other::erased_data_t>)           \
        : base_t(std::forward<Other>(other)),                                 \
          v_table_(static_cast<v_table_t const&>(other.v_table_)) {}          \
                                                                              \
    auto get_v_table_ptr(this auto& self) { return &self.v_table_; }          \
    _detail_ANYXX_METHODS(l)                                                  \
                                                                              \
        ~n() = default;                                                       \
    n() = default;                                                            \
    n(n const&) = default;                                                    \
    n(n&&) = default;                                                         \
    n& operator=(n const&) = default;                                         \
    n& operator=(n&&) = default;                                              \
    template <anyxx::is_erased_data Other>                                    \
    friend class anyxx::any_base;                                             \
    template <anyxx::is_any To, anyxx::is_any From>                           \
    friend To anyxx::unchecked_downcast_to(From from)                         \
      requires(std::derived_from<To, From>);                                  \
    template <anyxx::is_erased_data Other>                                    \
    using type_for = n<_detail_ANYXX_TEMPLATE_ARGS(tpl4)>;                    \
  };

#define ANY_INLINE_(n, BASE, l)                                           \
  ANY_INLINE_META_FUNCTION((ErasedData), (T), (Concrete), (Other), (), n, \
                           BASE, (), l)

#define ANY_INLINE(n, ...) ANY_INLINE_(n, ::anyxx::any_inline_base, __VA_ARGS__)

#define ANY_INLINE_TEMPLATE_(t, n, BASE, btpl, l)                              \
  ANY_INLINE_META_FUNCTION(_add_head((ErasedData), t), _add_head((T), t),      \
                           _add_head((Concrete), t), _add_head((Other), t), t, \
                           n, BASE, btpl, l)

#define ANY_INLINE_TEMPLATE(t, n, l) \
  ANY_INLINE_TEMPLATE_(t, n, ::anyxx::any_inline_base, (), l)

namespace {

ANY_INLINE(node_i, (ANY_METHOD(int, value, (), const),
                    ANY_METHOD(string, as_forth, (), const),
                    ANY_METHOD(string, as_lisp, (), const)))
using node = node_i<shared_const>;

struct Plus {
  Plus(node left, node right) : left(left), right(right) {}
  int value() const { return left.value() + right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " +";
  }
  string as_lisp() const {
    return "(plus " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Times {
  Times(node left, node right) : left(left), right(right) {}
  int value() const { return left.value() * right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " *";
  }
  string as_lisp() const {
    return "(times " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  int value() const { return int_; }
  string as_forth() const { return std::to_string(int_); }
  string as_lisp() const { return std::to_string(int_); }

  int int_;
};

template <typename NODE, typename... ARGS>
auto make_node(ARGS&&... args) {
  return node{std::make_shared<NODE>(std::forward<ARGS>(args)...)};
}

}  // namespace

TEST_CASE("21_Tree_any_inline") {
  using namespace anyxx;

  node n1 = make_node<Integer>(3);
  CHECK(n1.value() == 3);
  CHECK(n1.as_lisp() == "3");

  auto t = Plus{make_node<Integer>(1), make_node<Integer>(2)};
  node te{std::in_place, t};
  CHECK(te.value() == 7);
  //CHECK(expr1.as_lisp() == "7");

  auto expr = make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4)));
  REQUIRE(expr.value() == 14);

  REQUIRE(expr.value() == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  std::cout << "Ensure 'target_compile_options(examples_inline_mode PRIVATE "
               "/Ob2)' is used!\n";
  BENCHMARK("21_Tree any++ value") { return expr.value(); };
  BENCHMARK("21_Tree any++ as_lisp") { return expr.as_lisp(); };
#endif  // !_DEBUG
}

#endif