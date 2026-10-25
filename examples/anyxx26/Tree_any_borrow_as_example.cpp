#include <bit_factory/anyxx/ext/factory.hpp>
#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <string>

using std::string;

using namespace anyxx;

namespace _21_Tree_any_borrow_as {

template <typename Self>
struct node : anyxx::dynamic_deletable<Self> {
  int value() const;
};
template <typename Self>
struct serializeable : anyxx::dynamic_deletable<Self> {
  void serialize(std::ostream&) const;
};

std::ostream& operator<<(std::ostream& s,
                         anyxx::any_<cref, serializeable> const& any_) {
  any_.serialize(s);
  return s;
}

static factory<anyxx::any_<unique, serializeable>, std::string, std::istream&>
    deserialize_factory;

anyxx::any_<unique, serializeable> deserialize(std::istream& archive) {
  std::string type;
  archive >> type;
  return deserialize_factory(type, archive);
}
anyxx::any_<unique, node> deserialize_any_node(std::istream& archive) {
  return move_to<anyxx::any_<unique, node>>(deserialize(archive));
}
template <typename T>
auto register_deserialize_binary(std::string const& key) {
  return deserialize_factory.register_(key, [](std::istream& archive) {
    return T{deserialize_any_node(archive), deserialize_any_node(archive)};
  });
}

void serialize_binary(auto const& self, std::string_view key,
                      std::ostream& archive) {
  archive << key << " "
          << *borrow_as<anyxx::any_<cref, serializeable>>(self.left)
          << *borrow_as<anyxx::any_<cref, serializeable>>(self.right);
}

struct Plus {
  [[nodiscard]] int value() const { return left.value() + right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Plus ", archive);
  }
  anyxx::any_<unique, node> left, right;
};
auto __ = register_deserialize_binary<Plus>("Plus");

struct Times {
  [[nodiscard]] int value() const { return left.value() * right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Times ", archive);
  }
  anyxx::any_<unique, node> left, right;
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

namespace {
anyxx::castable_to<Plus, node> __;
anyxx::castable_to<Plus, serializeable> __;
anyxx::castable_to<Times, node> __;
anyxx::castable_to<Times, serializeable> __;
anyxx::castable_to<Integer, node> __;
anyxx::castable_to<Integer, serializeable> __;
};  // namespace

TEST_CASE("_21_Tree_any_borrow_as") {
  using namespace anyxx;
  using namespace _21_Tree_any_borrow_as;

  std::stringstream archive{
      "Plus Integer 1 Plus Times Integer 2 Integer 3 Integer 4 "};
  static_assert(anyxx::moveable_from<anyxx::any_<unique, node>::proxy_t,
                                     decltype(deserialize(archive))::proxy_t>);

  auto expr = move_to<anyxx::any_<unique, node>>(deserialize(archive));
  CHECK(expr.value() == 11);
  std::stringstream serialized;
  borrow_as<anyxx::any_<cref, serializeable>>(expr)->serialize(serialized);
  std::println("{}", serialized.str());
  auto expr2 = move_to<anyxx::any_<unique, node>>(deserialize(serialized));
  CHECK(expr2.value() == 11);
}
