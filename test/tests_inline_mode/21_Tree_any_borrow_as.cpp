// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <anyxx/anyxx.hpp>
#include <catch.hpp>
#include <iostream>
#include <print>
#include <string>

using std::cout;
using std::string;

using namespace anyxx;

namespace _21_Tree_any_borrow_as {

ANY(any_node, (ANY_CONST_METHOD(int, value)))

ANY(any_serializeable, (ANY_CONST_METHOD(void, serialize, std::ostream&)))
template <is_erased_data ErasedData>
std::ostream& operator<<(std::ostream& s,
                         any_serializeable<ErasedData> const& any) {
  any.serialize(s);
  return s;
}

factory<any_serializeable, std::string, std::istream&> deserialize_factory;

any_serializeable<unique> deserialize(std::istream& archive) {
  std::string type;
  archive >> type;
  return deserialize_factory.construct(type, archive);
}

struct Plus {
  int value() const { return left.value() + right.value(); }
  void serialize(std::ostream& archive) const {
    archive << "Plus " << *borrow_as<any_serializeable<const_observer>>(left)
            << *borrow_as<any_serializeable<const_observer>>(right);
  }
  any_node<unique> left, right;
};
auto __ = deserialize_factory.register_(
    "Plus", [](std::istream& archive) -> any_serializeable<unique> {
      return std::make_unique<Plus>(
          move_to<any_node<unique>>(deserialize(archive)),
          move_to<any_node<unique>>(deserialize(archive)));
    });

struct Times {
  int value() const { return left.value() * right.value(); }
  void serialize(std::ostream& archive) const {
    archive << "Times " << *borrow_as<any_serializeable<const_observer>>(left)
            << *borrow_as<any_serializeable<const_observer>>(right);
  }
  any_node<unique> left, right;
};
auto __ = deserialize_factory.register_(
    "Times", [](std::istream& archive) -> any_serializeable<unique> {
      return std::make_unique<Times>(
          move_to<any_node<unique>>(deserialize(archive)),
          move_to<any_node<unique>>(deserialize(archive)));
    });

struct Integer {
  int value() const { return int_; }
  void serialize(std::ostream& archive) const {
    archive << "Integer " << int_ << " ";
  }
  int int_;
};
auto __ = deserialize_factory.register_(
    "Integer", [](std::istream& archive) -> any_serializeable<unique> {
      auto integer = std::make_unique<Integer>();
      archive >> integer->int_;
      return integer;
    });

}  // namespace _21_Tree_any_borrow_as

using namespace _21_Tree_any_borrow_as;

ANY_REGISTER_MODEL(Plus, any_node);
ANY_REGISTER_MODEL(Plus, any_serializeable);
ANY_REGISTER_MODEL(Times, any_node);
ANY_REGISTER_MODEL(Times, any_serializeable);
ANY_REGISTER_MODEL(Integer, any_node);
ANY_REGISTER_MODEL(Integer, any_serializeable);

TEST_CASE("_21_Tree_any_borrow_as") {
  using namespace anyxx;
  using namespace _21_Tree_any_borrow_as;

  std::stringstream archive{
      "Plus Integer 1 Plus Times Integer 2 Integer 3 Integer 4 "};
  auto expr = move_to<any_node<unique>>(deserialize(archive));
  CHECK(expr.value() == 11);
  std::stringstream serialized;
  borrow_as<any_serializeable<const_observer>>(expr)->serialize(serialized);
  std::println("{}", serialized.str());
  auto expr2 = move_to<any_node<unique>>(deserialize(serialized));
  CHECK(expr2.value() == 11);
}
