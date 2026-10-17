// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ANY_SINGLETON.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <string>

using std::string;

using namespace anyxx;

namespace _21_Tree_any_borrow_as {

TRAIT(node, dynamic_deletable, (FN(int, value, (), const)))
TRAIT(serializeable, dynamic_deletable,
      (FN(void, serialize, (std::ostream&), const)))

template <is_proxy Proxy>
std::ostream& operator<<(std::ostream& s,
                         any<Proxy, serializeable> const& any) {
  any.serialize(s);
  return s;
}
ANY_SINGLETON_DECLARE(
    , deserialize_factory,
    factory<any<unique, serializeable>, std::string, std::istream&>);

any<unique, serializeable> deserialize(std::istream& archive) {
  std::string type;
  archive >> type;
  return deserialize_factory.construct(type, archive);
}
any<unique, node> deserialize_any_node(std::istream& archive) {
  return move_to<any<unique, node>>(deserialize(archive));
}
template <typename T>
auto register_deserialize_binary(std::string const& key) {
  return deserialize_factory.register_(key, [](std::istream& archive) {
    return T{deserialize_any_node(archive), deserialize_any_node(archive)};
  });
}

void serialize_binary(auto const& self, std::string_view key,
                      std::ostream& archive) {
  archive << key << " " << *borrow_as<any<cref, serializeable>>(self.left)
          << *borrow_as<any<cref, serializeable>>(self.right);
}

struct Plus {
  [[nodiscard]] int value() const { return left.value() + right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Plus ", archive);
  }
  any<unique, node> left, right;
};
auto __ = register_deserialize_binary<Plus>("Plus");

struct Times {
  [[nodiscard]] int value() const { return left.value() * right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Times ", archive);
  }
  any<unique, node> left, right;
};
auto __ = register_deserialize_binary<Times>("Times");

struct Integer {
  [[nodiscard]] int value() const { return int_; }
  void serialize(std::ostream& archive) const {
    archive << "Integer " << int_ << " ";
  }
  int int_ = 0;
};
auto __ = deserialize_factory.register_("Integer", [](std::istream& archive) {
  Integer integer;
  archive >> integer.int_;
  return integer;
});

}  // namespace _21_Tree_any_borrow_as

using namespace _21_Tree_any_borrow_as;

ANY_SINGLETON(_21_Tree_any_borrow_as, deserialize_factory)

ANY_REGISTER_MODEL(Plus, node);
ANY_REGISTER_MODEL(Plus, serializeable);
ANY_REGISTER_MODEL(Times, node);
ANY_REGISTER_MODEL(Times, serializeable);
ANY_REGISTER_MODEL(Integer, node);
ANY_REGISTER_MODEL(Integer, serializeable);

TEST_CASE("_21_Tree_any_borrow_as") {
  using namespace anyxx;
  using namespace _21_Tree_any_borrow_as;

  std::stringstream archive{
      "Plus Integer 1 Plus Times Integer 2 Integer 3 Integer 4 "};
  auto expr = move_to<any<unique, node>>(deserialize(archive));
  CHECK(expr.value() == 11);
  std::stringstream serialized;
  borrow_as<any<cref, serializeable>>(expr)->serialize(serialized);
  std::println("{}", serialized.str());
  auto expr2 = move_to<any<unique, node>>(deserialize(serialized));
  CHECK(expr2.value() == 11);
}
