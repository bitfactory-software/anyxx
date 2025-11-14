#include <anyxx/anyxx.hpp>
#include <catch.hpp>
#include <string>

using namespace Catch::Matchers;

using namespace anyxx;

ANY_HAS_DISPATCH(, test_base_i)
ANY_HAS_DISPATCH(, test_derived_i)
namespace {
ANY(test_base_i, (ANY_METHOD(std::string, to_string, (), const)))
ANY_(test_derived_i, test_base_i,
     (ANY_METHOD(void, from_string, (std::string const&))))

}  // namespace

namespace {
struct x_t {
  std::string s_;
};
}  // namespace

using test_base_i_co = test_base_i<const_observer>;
using test_derived_i_mo = test_derived_i<mutable_observer>;

template <>
struct test_base_i_concept_map<x_t> {
  static auto to_string(x_t const& self) { return self.s_; }
};
template <>
struct test_derived_i_concept_map<x_t> {
  static void from_string(x_t& self, std::string_view s) { self.s_ = s; }
};

dispatch<std::string(virtual_<test_base_i_co>)> to_string_otherwise;
auto __ = to_string_otherwise.define<x_t>(
    [](auto& expr) { return expr.s_ + " otherwise"; });

dispatch<void(virtual_<test_derived_i_mo>, std::string const&)>
    from_string_otherwise;
auto __ =
    from_string_otherwise.define<x_t>([](auto& expr, std::string const& s) {
      expr.s_ = std::string{"otherwise "} + s;
    });

auto base_table = dispatch_table_instance<test_base_i_v_table, x_t>();
auto derived_table = dispatch_table_instance<test_derived_i_v_table, x_t>();

namespace {

TEST_CASE("dispatch") {
  CHECK(base_table->size() == 1);
  CHECK(derived_table->size() == 1);

  CHECK(test_base_i_v_table::imlpementation<x_t>()
            ->own_dispatch_holder_t::dispatch_table);
  CHECK(test_derived_i_v_table::imlpementation<x_t>()
            ->own_dispatch_holder_t::dispatch_table);

  x_t x{"hallo"};
  test_derived_i_mo i{x};
  CHECK(i.to_string() == "hallo");
  CHECK(to_string_otherwise(i) == "hallo otherwise");
}

}  // namespace
