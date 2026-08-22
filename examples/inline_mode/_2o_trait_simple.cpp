#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace simple {

TRAIT_(trait1, anyxx::dynamic_copyable, (ANY_FN(std::string, fn1, (), const)))

std::string cat(std::vector<anyxx::any<trait1, anyxx::val<>>> const& items) {
  std::string result;
  for (auto const& item : items) {
    result += item.fn1();
  }
  return result;
}

struct model1 {
  std::string s;
  [[nodiscard]] std::string fn1() const { return "model1"; }
};

struct model2 {
  int x;
};

}  // namespace simple

ANY_MODEL_MAP((simple::model2), simple::trait1) {
  static std::string fn1(simple::model2 const& m) {
    return std::to_string(m.x);
  };
};

TEST_CASE("any via concept simple") {
  using namespace anyxx;
  using namespace simple;

  any<trait1, val<>> test1{model1{"model1"}};
  CHECK(test1.fn1() == "model1");
  any<trait1, val<>> test2{model2{42}};
  CHECK(test2.fn1() == "42");

  CHECK(cat({test1, test2}) == "model142");
}