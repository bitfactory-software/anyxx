// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <string>

using std::cout;
using std::string;

using namespace anyxx;

namespace _21_Tree_any_borrow_as {

ANY(any_value, (ANY_METHOD(int, value, (), const)))

ANY(any_serializeable, (ANY_METHOD(void, serialize, (std::ostream&), const)))
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
any_value<unique> deserialize_any_node(std::istream& archive) {
  return move_to<any_value<unique>>(deserialize(archive));
}
template <typename T>
auto register_deserialize_binary(std::string const& key) {
  return deserialize_factory.register_(
      key, [](std::istream& archive) -> any_serializeable<unique> {
        return std::make_unique<T>(deserialize_any_node(archive),
                                   deserialize_any_node(archive));
      });
}

void serialize_binary(auto const& self, std::string_view key,
                      std::ostream& archive) {
  archive << key << " "
          << *borrow_as<any_serializeable<const_observer>>(self.left)
          << *borrow_as<any_serializeable<const_observer>>(self.right);
}

struct Plus {
  int value() const { return left.value() + right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Plus ", archive);
  }
  any_value<unique> left, right;
};
auto __ = register_deserialize_binary<Plus>("Plus");

struct Times {
  int value() const { return left.value() * right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Times ", archive);
  }
  any_value<unique> left, right;
};
auto __ = register_deserialize_binary<Times>("Times");

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

ANY_REGISTER_MODEL(Plus, any_value);
ANY_REGISTER_MODEL(Plus, any_serializeable);
ANY_REGISTER_MODEL(Times, any_value);
ANY_REGISTER_MODEL(Times, any_serializeable);
ANY_REGISTER_MODEL(Integer, any_value);
ANY_REGISTER_MODEL(Integer, any_serializeable);

TEST_CASE("_21_Tree_any_borrow_as") {
  using namespace anyxx;
  using namespace _21_Tree_any_borrow_as;

  std::stringstream archive{
      "Plus Integer 1 Plus Times Integer 2 Integer 3 Integer 4 "};
  auto expr = move_to<any_value<unique>>(deserialize(archive));
  CHECK(expr.value() == 11);
  std::stringstream serialized;
  borrow_as<any_serializeable<const_observer>>(expr)->serialize(serialized);
  std::println("{}", serialized.str());
  auto expr2 = move_to<any_value<unique>>(deserialize(serialized));
  CHECK(expr2.value() == 11);
}
