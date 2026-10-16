#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v23/anys/ranges.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("range static assert") {
  using namespace anyxx;

  static_assert(std::input_iterator<any_<forward_iterator, int, int>>);
  static_assert(std::input_iterator<any_<forward_iterator, int, int&>>);
  static_assert(std::input_iterator<any_input_iterator<int, int>>);
  static_assert(std::input_iterator<any_input_iterator<int>>);
  static_assert(is_compareable_iterator<forward_iterator, iterator_val, int, int>);
  static_assert(std::forward_iterator<any_forward_iterator<int, int>>);
  static_assert(std::forward_iterator<any_forward_iterator<int>>);
  static_assert(
      std::bidirectional_iterator<any_bidirectional_iterator<int, int>>);
  static_assert(
      std::bidirectional_iterator<any_bidirectional_iterator<int>>);
  static_assert(
      std::random_access_iterator<any_random_access_iterator<int, int>>);
  static_assert(
      std::random_access_iterator<any_random_access_iterator<int>>);
  static_assert(is_any_self_forward_range<any_self_forward_range>);

  static_assert(
      any_forward_iterator<self, self>::v_table_t::val_is_nullable::value ==
      true);
  static_assert(
      std::same_as<any_forward_iterator<self, self>::v_table_t::any_value_t,
                   any<nullable_val<iterator_val_proxy_size>, forward_iterator,
                       self, self>>);
  static_assert(
      std::same_as<random_access_iterator<self, self>::v_table_t::any_value_t,
                   any<nullable_val<iterator_val_proxy_size>,
                       random_access_iterator, self, self>>);
  static_assert(std::forward_iterator<any_forward_iterator<self, self>>);


  static_assert(is_sentinel_trait_for<sentinel, input_iterator, int, int>);
  static_assert(is_sentinel_trait_for<sentinel, input_iterator, int, int const&>);
  static_assert(is_sentinel_trait_for<sentinel, random_access_iterator, int, int>);
  static_assert(
      is_sentinel_trait_for<sentinel, random_access_iterator, int, int const&>);
  static_assert(
      std::sentinel_for<any_<sentinel, int, int>, any_<input_iterator, int, int>>);
  static_assert(std::sentinel_for<any_<sentinel, int, const int&>,
      any_<input_iterator, int, const int&>>);

  static_assert(std::sized_sentinel_for<any_<sized_sentinel, int, int&>,
      any_<bidirectional_iterator, int, int&>>);
  static_assert(std::sentinel_for<any_<sentinel, int, int&>,
      any_<input_iterator, int, int&>>);

  static_assert(std::ranges::forward_range<any_forward_range<self, self>>);
}
