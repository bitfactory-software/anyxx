#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace anyxx {

ANY_TEMPLATE_EX(((ValueType), (Reference)), any_forward_iterator,
                (ANY_OP(anyxx::self &, ++, (), ),
                 ANY_OP(anyxx::self, ++, (int), ),
                 ANY_OP(Reference, *, (), const),
                 ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const), const,
                                  ([&x](auto const &r) { return x == r; }))),
                anyxx::value, ,
                (using iterator_category = std::forward_iterator_tag;
                 using difference_type = std::ptrdiff_t;
                 using value_type = ValueType; using reference = Reference;))

}  // namespace anyxx

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
    CHECK(b == e);
    CHECK(!(b != e));
    static_assert(std::movable<any_forward_iterator<int, int>>);
    static_assert(
        std::same_as<decltype(++b), any_forward_iterator<int, int> &>);
    static_assert(std::forward_iterator<any_forward_iterator<int, int>>);
  }
  {
    v_t v{1, 2, 3};
    any_forward_iterator<int, int> b{v.begin()};
    any_forward_iterator<int, int> e{v.end()};
    CHECK(b != e);
    int x = 0;
    for (auto i = b; i != e; ++i) CHECK(*i == v[x++]);
  }
}
