#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace anyxx;

namespace {

struct test_base_i_has_open_dispatch {};
struct test_derived_i_has_open_dispatch {};

ANY(test_base_i, (ANY_METHOD(std::string, to_string, (), const)), )
ANY_(test_derived_i, test_base_i,
     (ANY_METHOD(void, from_string, (std::string const&), )), )

struct x_t {
  std::string s_;
};

using test_base_i_co = test_base_i<const_observer>;
using test_derived_i_mo = test_derived_i<mutable_observer>;

ANY_MODEL_MAP((x_t), test_base_i){
    static auto to_string(x_t const& self){return self.s_;
}  // namespace
}
;
ANY_MODEL_MAP((x_t), test_derived_i){
    static void from_string(x_t & self, std::string_view s){self.s_ = s;
}
}
;

dispatch<std::string(virtual_<test_base_i_co>)> to_string_otherwise;
auto __ = to_string_otherwise.define<x_t>(
    [](auto& expr) { return expr.s_ + " otherwise"; });

dispatch<void(virtual_<test_derived_i_mo>, std::string const&)>
    from_string_otherwise;
auto __ =
    from_string_otherwise.define<x_t>([](auto& expr, std::string const& s) {
      expr.s_ = std::string{"otherwise "} + s;
    });

auto base_table = dispatch_table_instance<test_base_i_v_table<anyxx::dyn>, x_t>();
auto derived_table = dispatch_table_instance<test_derived_i_v_table<anyxx::dyn>, x_t>();

TEST_CASE("dispatch") {
  CHECK(base_table->size() == 1);
  CHECK(derived_table->size() == 1);

  CHECK(test_base_i_v_table<anyxx::dyn>::imlpementation<x_t>()
            ->own_dispatch_holder_t::dispatch_table);
  CHECK(test_derived_i_v_table<anyxx::dyn>::imlpementation<x_t>()
            ->own_dispatch_holder_t::dispatch_table);

  x_t x{"hallo"};
  test_derived_i_mo i{x};
  CHECK(i.to_string() == "hallo");
  CHECK(to_string_otherwise(i) == "hallo otherwise");
}

}  // namespace
