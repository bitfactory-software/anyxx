#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>

namespace anyxx {

TRAIT_TEMPLATE(
    ((Tag)), strong_typedef_trait,
    (ANY_OP_MAP_NAMED(anyxx::self, +, op_plus, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, -, op_minus, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, *, op_multiplies, (anyxx::self const &),
                      const),
     ANY_OP_MAP_NAMED(anyxx::self, /, op_divides, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, %, op_modulus, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, ^, op_bit_xor, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, &, op_bit_and, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, |, op_bit_or, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, ~, op_bit_not, (), const),
     ANY_OP_MAP_NAMED(anyxx::self, !, op_logical_not, (), const),
     ANY_OP_MAP_NAMED(anyxx::self, +=, op_plus_assign, (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, -=, op_minus_assign,
                      (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, *=, op_multiplies_assign,
                      (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, /=, op_divides_assign,
                      (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, ^=, op_bit_xor_assign,
                      (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, &=, op_bit_and_assign,
                      (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, |=, op_bit_or_assign,
                      (anyxx::self const &), ),
     ANY_OP_MAP_NAMED(anyxx::self, ==, op_equal_to, (anyxx::self const &),
                      const),
     ANY_OP_MAP_NAMED(anyxx::self, !=, op_not_equal_to, (anyxx::self const &),
                      const),
     ANY_OP_MAP_NAMED(anyxx::self, <=, op_less_equal, (anyxx::self const &),
                      const),
     ANY_OP_MAP_NAMED(anyxx::self, >=, op_greater_equal, (anyxx::self const &),
                      const),
     ANY_OP_MAP_NAMED(anyxx::self, <, op_less, (anyxx::self const &), const),
     ANY_OP_MAP_NAMED(anyxx::self, ++, op_inc_prefix, (), )/*,
     ANY_OP_MAP_NAMED(anyxx::self, ++(int), op_inc_postfix, (), )*/))

template <typename T, typename Tag>
  requires std::is_arithmetic_v<T>
struct strong_typedef_trait_model_map<T, Tag>
    : strong_typedef_trait_default_model_map<T, Tag> {
  static auto op_plus(T const rhs, T const lhs) { return rhs + lhs; }
  static auto op_minus(T const &l, T const &r) { return l - r; }
  static auto op_multiplies(T const &l, T const &r) { return l * r; }
  static auto op_divides(T const &l, T const &r) { return l / r; }
  static auto op_modulus(T const &l, T const &r) { return l % r; }
  static auto op_bit_xor(T const &l, T const &r) { return l ^ r; }
  static auto op_bit_and(T const &l, T const &r) { return l & r; }
  static auto op_bit_or(T const &l, T const &r) { return l | r; }
  static auto op_bit_not(T const &l) { return ~l; }
  static auto op_logical_not(T const &l) { return !l; }
  static auto op_plus_assign(T &l, T const &r) { return l += r; }
  static auto op_minus_assign(T &l, T const &r) { return l -= r; }
  static auto op_multiplies_assign(T &l, T const &r) { return l *= r; }
  static auto op_divides_assign(T &l, T const &r) { return l /= r; }
  static auto op_bit_xor_assign(T &l, T const &r) { return l ^= r; }
  static auto op_bit_and_assign(T &l, T const &r) { return l &= r; }
  static auto op_bit_or_assign(T &l, T const &r) { return l |= r; }
  static auto op_equal_to(T const &l, T const &r) { return l == r; }
  static auto op_not_equal_to(T const &l, T const &r) { return l != r; }
  static auto op_less_equal(T const &l, T const &r) { return l <= r; }
  static auto op_greater_equal(T const &l, T const &r) { return l >= r; }
  static auto op_less(T const &l, T const &r) { return l < r; }
  static auto op_greater(T const &l, T const &r) { return l > r; }
  static auto op_inc_prefix(T &l) { return ++l; }
  static auto op_inc_postfix(T &l) { return l++; }
};

template <typename T, typename Tag>
using strong_typedef = any<using_<T>, strong_typedef_trait<Tag>>;

}  // namespace anyxx

TEST_CASE("strong typedef") {
  using typename_1 = anyxx::strong_typedef<int, struct tag1>;
  using typename_2 = anyxx::strong_typedef<int, struct tag2>;

  typename_1 a{1}, b{2};
  auto c = a + b;
  static_assert(std::same_as<decltype(c), typename_1>);
  CHECK(get_proxy_value(c) == 3);
  auto d = c += b;
  CHECK(get_proxy_value(c) == 5);
  CHECK(get_proxy_value(d) == 5);
  static_assert(std::same_as<decltype(d), typename_1>);
  (d = c += b) = a;
  CHECK(get_proxy_value(d) == 1);
  auto e = ++a;
  CHECK(get_proxy_value(e) == 2);
  CHECK(get_proxy_value(e) == 2);
  //auto e = a++;

  // The following lines fail to compile, as expected!
  // typename_2 a2{a};
  // typename_2 a2 = a;
}
