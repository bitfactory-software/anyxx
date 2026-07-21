#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <print>

namespace example_3 {

anyxx::any_range<anyxx::any_forward_iterator<int, int const&>> a_range(
    bool use_list) {
  static std::vector<int> v = {1, 2, 3};
  static std::list<int> l = {4, 5, 6};
  if (use_list)
    return l;
  else
    return v;
}

anyxx::any_range<anyxx::any_forward_iterator<int, int const&>,
                         anyxx::val<>>
a_range_value(bool use_list) {
  if (use_list)
    return std::list<int>{4, 5, 6};
  else
    return std::vector<int>{1, 2, 3};
}

TRAIT_(stringable, anyxx::dynamic_value,
       (ANY_FN_DEF(public, std::string, to_string, (), const,
                   [&x]() { return std::format("{}", x); })))

template <typename Proxy>
using any_stringable = anyxx::any<stringable, Proxy>;

TRAIT_(node, anyxx::dynamic_value,
       (ANY_FN_DEF(public, anyxx::self, sum,
                   ((anyxx::any_self_forward_range const&)), const,
                   [&x](auto const& r) {
                     auto s = x;
                     for (auto i : r) {
                       s += i;
                     }
                     return s;
                   })));
template <typename Proxy = anyxx::val<>>
using any_node = anyxx::any<node, Proxy>;

}  // namespace example_3

TEST_CASE("example3 simple lifetime") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v0{};
    any_range<any_forward_iterator<int, int>> r0{v0};
    {
      r0.begin();
    }
  }
}

TEST_CASE(
    "example 3 any_forward_iterator (concrete value_type, erased iterator)") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v0{};
    any_range<anyxx::any_forward_iterator<int, int>> r0{v0};
    {
      any_forward_iterator<int, int> b{r0.begin()};
      any_forward_iterator<int, int> e{r0.end()};
      CHECK(b == e);
    }
  }
  using it = v_t::iterator;
  {
    v_t v;
    any_forward_iterator<int, int> b{v.begin()};
    any_forward_iterator<int, int> e{v.end()};
    CHECK(b == e);
    CHECK(!(b != e));
    static_assert(std::movable<any_forward_iterator<int, int>>);
    static_assert(std::same_as<decltype(++b), any_forward_iterator<int, int>&>);
    static_assert(std::forward_iterator<any_forward_iterator<int, int>>);
  }
  {
    v_t v{1, 2, 3};
    {
      any_forward_iterator<int, int> b{v.begin()};
      any_forward_iterator<int, int> e{v.end()};
      CHECK(b != e);
      CHECK(!(b == e));
      int x = 0;
      for (auto i = b; i != e; ++i) {
        CHECK(*i == v[x++]);
      }
      CHECK(x == 3);
    }
    any_range<any_forward_iterator<int, int>> r{v};
    int x = 0;
    for (auto i : r) {
      CHECK(i == v[x++]);
    }
    CHECK(x == 3);
  }
  {
    int x = 1;
    for (auto i : a_range(false)) CHECK(i == x++);
    for (auto i : a_range(true)) CHECK(i == x++);
    CHECK(x == 7);
  }
  {
    int x = 1;
    for (auto i : a_range_value(false)) CHECK(i == x++);
    for (auto i : a_range_value(true)) CHECK(i == x++);
    CHECK(x == 7);
  }
}

TEST_CASE(
    "example 3 any_forward_iterator (concrete value_type, concrete "
    "iterator)") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any<range<any_forward_iterator<int, int>>, using_<v_t const&>> r{v};
    int x = 0;
    for (auto i : r) CHECK(i == v[x++]);
    CHECK(x == 3);
  }
}

TEST_CASE("example 3 any_forward_iterator (any value_type, erased iterator)") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any_range<any_forward_iterator<any_stringable<anyxx::val<>>,
                                           any_stringable<anyxx::val<>>>>
        r{v};
    int x = 0;
    for (auto i : r) CHECK(i.to_string() == std::to_string(v[x++]));
    CHECK(x == 3);
  }
}

TEST_CASE(
    "example 3 any_forward_iterator (any value_type, concrete iterator) "
    "only "
    "theory, not praxis relevant") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    anyxx::any<anyxx::range<any_forward_iterator<
                   any_stringable<anyxx::val<>>, any_stringable<anyxx::val<>>>>,
               anyxx::using_<v_t const&>>
        r{v};
    int x = 0;
    for (auto i : r) {
      CHECK(i.to_string() == std::to_string(v[x++]));
    }
    CHECK(x == 3);
  }
}

TEST_CASE("example 3 transform unerase") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any_range<any_forward_iterator<any_stringable<anyxx::val<>>,
                                           any_stringable<anyxx::val<>>>>
        r{v};
    int x = 0;
    for (auto i : std::views::transform(
             r, [](any_stringable<anyxx::val<>> const& v) -> int {
               return *anyxx::unerase_cast<int>(v);
             })) {
      std::println("{}", i);
      CHECK(i == ++x);
    }
    CHECK(x == 3);
  }
}

TEST_CASE("example 3 self in range") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  std::vector<int> v = {1, 2, 3};
  {
    any_node<val<>> n1{0};
    auto r = n1.sum(v);
    CHECK(*unerase_cast<int>(r) == 6);
  }
  {
    any_node<using_<int>> n1{0};
    auto r = n1.sum(v);
    CHECK(get_proxy_value(r) == 6);
  }
  {
    any_range<any_forward_iterator<any_node<>, any_node<>>,
                      anyxx::val<>>
        r{v};
    {
      any_node<using_<int>> n1{0};
      auto result = n1.sum(r);
      CHECK(get_proxy_value(result) == 6);
    }
    {
      any_node<> n1{0};
      auto result = n1.sum(r);
      CHECK(*unerase_cast<int>(result) == 6);
    }
  }
}

TEST_CASE("example 3 static any range of view") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  auto v = std::views::iota(0, 3);
  auto v2 =
      v | std::views::transform([](auto i) { return static_cast<int>(i); });
  static_assert(std::is_trivially_move_constructible_v<decltype(v2)>);
  static_assert(std::is_trivially_move_assignable_v<decltype(v2)>);
  static_assert(std::is_trivially_copy_constructible_v<decltype(v2)>);
  static_assert(std::is_trivially_copy_assignable_v<decltype(v2)>);
  static_assert(sizeof(v2) <= anyxx::small_object_size);
  any_range<any_forward_iterator<using_<int>::as<stringable>,
                                         using_<int>::as<stringable>>>
      r{v2};
  std::string result;
  for (auto i : r) {
    result += i.to_string();
  }
  CHECK(result == "012");
}

// iterator adapter like boost::iterator_adaptor based on
// anyxx::forward_iterator Work in progress!!!

namespace {
struct node_base {
  node_base() : m_next(0) {}
  // Each node manages all of its tail nodes
  virtual ~node_base() { delete m_next; }
  // Access the rest of the list
  node_base* next() const { return m_next; }
  // print to the stream
  virtual void print(std::ostream& s) const = 0;
  // double the value
  virtual void double_me() = 0;
  void append(node_base* p) {
    if (m_next)
      m_next->append(p);
    else
      m_next = p;
  }

 private:
  node_base* m_next;
};

inline std::ostream& operator<<(std::ostream& s, node_base const& n) {
  n.print(s);
  return s;
}

template <class T>
struct node : node_base {
  node(T x) : m_value(x) {}
  void print(std::ostream& s) const { s << this->m_value; }
  void double_me() { m_value += m_value; }

 private:
  T m_value;
};

}  // namespace

namespace anyxx {
template <typename ValueType, typename Reference>
struct forward_iterator_model_map<node_base*, ValueType, Reference>
    : anyxx::forward_iterator_default_model_map<node_base*, ValueType,
                                                Reference> {
  node_base* op_pre_increment(node_base*& x) const { return x = x->next(); }
};
}  // namespace anyxx

namespace {
using node_iterator = anyxx::using_<node_base*>::as<
    anyxx::forward_iterator<node_base*, node_base&>>;
using node_const_iterator = anyxx::using_<node_base*>::as<
    anyxx::forward_iterator<node_base const*, const node_base&>>;
}  // namespace

TEST_CASE("example 3 iterator adaptor ") {
  std::unique_ptr<node<int>> nodes(new node<int>(42));
  nodes->append(new node<std::string>("is greater than:"));
  nodes->append(new node<int>(13));

  std::stringstream out1;
  std::copy(node_iterator(nodes.get()), node_iterator(),
            std::ostream_iterator<node_base>(out1, " "));
  CHECK(out1.str() == "42 is greater than: 13 ");
  std::for_each(node_iterator(nodes.get()), node_iterator(),
                [](auto& node) { node.double_me(); });
  std::stringstream out2;
  std::copy(node_const_iterator(nodes.get()), node_const_iterator(),
            std::ostream_iterator<node_base>(out2, "/"));
  CHECK(out2.str() == "84/is greater than:is greater than:/26/");
}