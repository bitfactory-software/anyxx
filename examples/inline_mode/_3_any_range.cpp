#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace anyxx {

// ANY_TEMPLATE(((ValueType), (Reference)), any_map,
//              (ANY_METHOD(Reference, dereference, (), const)), anyxx::value, )

// ANY_TEMPLATE_EX(((ValueType), (Reference)), any_forward_iterator,
//                 (ANY_OP_DEFAULTED(anyxx::self &, ++, inc_prefix, (), ,
//                                   ([&x]() -> decltype(auto) { return ++x;
//                                   })),
//                  ANY_OP_DEFAULTED(anyxx::self, ++, inc_postfix, (int), ,
//                                   ([&x]() -> decltype(auto) { return x++;
//                                   })),
//                  ANY_METHOD_DEFAULTED(int, dereference, (), const,
//                                       ([&x]() -> auto { return *x; })),
//                  ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const),
//                  const,
//                                   ([&x](auto const &r) { return x == r; }))),
//                 anyxx::value, ,
//                 (using iterator_category = std::forward_iterator_tag;
//                  using difference_type = std::ptrdiff_t;
//                  using value_type = ValueType; using reference = Reference;
//                  ))

ANY_TEMPLATE_EX(((ValueType), (Reference)), any_forward_iterator,
                (ANY_OP_DEFAULTED(anyxx::self &, ++, inc_prefix, (), ,
                                  ([&x]() -> decltype(auto) { return ++x; })),
                 ANY_OP_DEFAULTED(anyxx::self, ++, inc_postfix, (int), ,
                                  ([&x](int) -> decltype(auto) {
                                    return x++;
                                  })),
                 ANY_OP_DEFAULTED(Reference, *, dereference, (), const,
                                  ([&x]() -> auto { return *x; })),
                 ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const), const,
                                  ([&x](auto const &r) { return x == r; }))),
                anyxx::value, ,
                (using iterator_category = std::forward_iterator_tag;
                 using difference_type = std::ptrdiff_t;
                 using value_type = ValueType; using reference = Reference;))

// any_t operator++(int) {
//   auto old = *this;
//   ++(*this);
//   return old;

}  // namespace anyxx
/*(using iterator_category = std::forward_iterator_tag;
                  using difference_type = std::ptrdiff_t;
                  using value_type = ValueType;
                  using reference = Reference;)*/

namespace example_3 {}  // namespace example_3

TEST_CASE("example 3a any_forward_iterator") {
  using namespace anyxx;
  using namespace std::string_literals;

  using v_t = std::vector<int>;
  using it = v_t::iterator;
  {
      v_t v;
      any_forward_iterator<int, int> b{v.begin()};
      any_forward_iterator<int, int> e{v.end()};
      CHECK(b==e);
      static_assert(std::movable<any_forward_iterator<int, int>>);
      static_assert(std::same_as<decltype(++b), any_forward_iterator<int, int> &>);
      static_assert(std::forward_iterator<any_forward_iterator<int, int>>);
  }
  //{
  //    v_t v{1, 2, 3};
  //    for( auto i = )
  //    any_forward_iterator<int, int> b{v.begin()};
  //    any_forward_iterator<int, int> e{v.()};
  //    CHECK(b==e);
  //    static_assert(std::movable<any_forward_iterator<int, int>>);
  //    static_assert(std::same_as<decltype(++b), any_forward_iterator<int, int> &>);
  //    static_assert(std::forward_iterator<any_forward_iterator<int, int>>);
  //}
}
