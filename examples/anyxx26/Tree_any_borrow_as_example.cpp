#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/dyn.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <string>

using std::string;

using namespace anyxx;

namespace _21_Tree_any_borrow_as {

template <anyxx26::is_trait Trait, typename Self>
struct node : anyxx26::dynamic_deletable<Trait, Self> {
    int value() const;
};
template <anyxx26::is_trait Trait, typename Self>
struct serializeable : anyxx26::dynamic_deletable<Trait, Self> {
    void serialize(std::ostream&) const;
};

std::ostream& operator<<(std::ostream& s, anyxx26::dyn<serializeable, cref> const& any) {
  any.serialize(s);
  return s;
}
ANY_SINGLETON_DECLARE(, deserialize_factory,
                      factory<anyxx26::dyn<serializeable, unique>, std::string, std::istream&>);

anyxx26::dyn<serializeable, unique> deserialize(std::istream& archive) {
  std::string type;
  archive >> type;
  return deserialize_factory.construct(type, archive);
}
anyxx26::dyn<node, unique> deserialize_any_node(std::istream& archive) {
  return move_to<anyxx26::dyn<node, unique>>(deserialize(archive));
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
          << *borrow_as<anyxx26::dyn<serializeable, cref>>(self.left)
          << *borrow_as<anyxx26::dyn<serializeable, cref>>(self.right);
}

struct Plus {
  [[nodiscard]] int value() const { return left.value() + right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Plus ", archive);
  }
  anyxx26::dyn<node, unique> left, right;
};
auto __ = register_deserialize_binary<Plus>("Plus");

struct Times {
  [[nodiscard]] int value() const { return left.value() * right.value(); }
  void serialize(std::ostream& archive) const {
    serialize_binary(*this, "Times ", archive);
  }
  anyxx26::dyn<node, unique> left, right;
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

ANY26_REGISTER_MODEL(Plus, node);
ANY26_REGISTER_MODEL(Plus, serializeable);
ANY26_REGISTER_MODEL(Times, node);
ANY26_REGISTER_MODEL(Times, serializeable);
ANY26_REGISTER_MODEL(Integer, node);
ANY26_REGISTER_MODEL(Integer, serializeable);

TEST_CASE("_21_Tree_any_borrow_as") {
  using namespace anyxx;
  using namespace _21_Tree_any_borrow_as;

  std::stringstream archive{
      "Plus Integer 1 Plus Times Integer 2 Integer 3 Integer 4 "};
  static_assert(anyxx::moveable_from<anyxx26::dyn<node, unique>::proxy_t, decltype(deserialize(archive))::proxy_t>);

  auto expr = move_to<anyxx26::dyn<node, unique>>(deserialize(archive));
  CHECK(expr.value() == 11);
  std::stringstream serialized;
  borrow_as<anyxx26::dyn<serializeable, cref>>(expr)->serialize(serialized);
  std::println("{}", serialized.str());
  auto expr2 = move_to<anyxx26::dyn<node, unique>>(deserialize(serialized));
  CHECK(expr2.value() == 11);
}
