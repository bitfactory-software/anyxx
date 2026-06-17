#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef _MSC_VER
#pragma warning( \
    disable : 4189)  // local variable is initialized but not referenced
#endif

using namespace anyxx;
using namespace std::literals;

namespace {
TRAIT_EX(thing, (ANY_FN(std::string, name, (), const)), , , (ANY_OPEN_DISPATCH),
         ())
template <typename Proxy = anyxx::shared>
using any_thing = anyxx::any<Proxy, thing>;

}  // namespace

namespace {}  // namespace

namespace {

namespace {
struct Asteroid {
  static std::string name() { return "Asteroid"; }
};
}  // namespace
namespace {
struct Spaceship {
  static std::string name() { return "Spaceship"; }
};

}  // namespace

TEST_CASE("multi_dispatch 1") {
  dispatch<std::string(virtual_<any_thing<cref>>, virtual_<any_thing<cref>>)>
      collide;

  collide.define<Asteroid, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });

  CHECK(v_table_instance<thing_v_table, Asteroid>()->dispatch_table->size() ==
        3);
  CHECK(v_table_instance<thing_v_table, Spaceship>()->dispatch_table->size() ==
        3);

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid->Spaceship");
  CHECK(collide(thing_asteroid, thing_asteroid) == "Asteroid->Asteroid");
  CHECK(collide(thing_spaceship, thing_spaceship) == "Spaceship->Spaceship");
  CHECK(collide(thing_spaceship, thing_asteroid) == "Spaceship->Asteroid");
}

TEST_CASE("multi_dispatch 2") {
  dispatch<std::string(virtual_<any_thing<cref>>, virtual_<any_thing<cref>>)>
      collide;

  collide.define<Asteroid, any_thing<cref>>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + ":A->*" + r.name();
      });
  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid:A->*Spaceship");
  CHECK(collide(thing_asteroid, thing_asteroid) == "Asteroid->Asteroid");
  CHECK(collide(thing_spaceship, thing_spaceship) == "Spaceship->Spaceship");
  CHECK(collide(thing_spaceship, thing_asteroid) == "Spaceship->Asteroid");
}

TEST_CASE("multi_dispatch 3") {
  dispatch<std::string(virtual_<any_thing<cref>>, virtual_<any_thing<cref>>)>
      collide;

  collide.define<Asteroid, any_thing<cref>>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + ":A->*" + r.name();
      });
  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<any_thing<cref>, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->S:" + r.name();
      });
  collide.define<any_thing<cref>, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->A:" + r.name();
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid:A->*Spaceship");
  CHECK(collide(thing_asteroid, thing_asteroid) == "Asteroid->Asteroid");
  CHECK(collide(thing_spaceship, thing_spaceship) == "Spaceship*->S:Spaceship");
  CHECK(collide(thing_spaceship, thing_asteroid) == "Spaceship*->A:Asteroid");
}

TEST_CASE("multi_dispatch 4") {
  dispatch<std::string(virtual_<any_thing<cref>>, virtual_<any_thing<cref>>)>
      collide;

  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto a, [[maybe_unused]] auto s) { return "a->a"; });
  collide.define<any_thing<cref>, Spaceship>(
      [](auto const& any, [[maybe_unused]] auto const& s) {
        return get_type_info(any).name() + "->s"s;
      });
  collide.define<any_thing<cref>, Asteroid>(
      []([[maybe_unused]] auto const& any, [[maybe_unused]] auto const& a) {
        return "any->a";
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship).contains("Asteroid"));
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship).contains("Spaceship"));
  CHECK(collide(thing_spaceship, thing_asteroid) == "any->a");
}

}  // namespace

// some compile time self tests

namespace {
ANY(Dummy, , )

static_assert(!is_open_dispatch_v_table<Dummy::v_table_t>);
static_assert(is_open_dispatch_v_table<thing::v_table_t>);

}  // namespace
