#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/dispatch_table.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef _MSC_VER
#pragma warning(disable \
                : 4189)  // local variable is initialized but not referenced
#endif

using namespace anyxx;
using namespace std::literals;

namespace {
TRAIT_EX(thing, anyxx::dynamic_copyable, (FN(std::string, name, (), const)), , ,
         (HAS_OPEN_DISPATCH), ())
template <typename Proxy = anyxx::shared>
using any_thing = anyxx::any_<Proxy, thing>;

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
  auto has_conflicts = !collide.define_<any_thing<cref>, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->S:" + r.name();
      });
  CHECK(has_conflicts);
  auto has_conflicts1 = collide.define_<Asteroid, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return "Asteroid->Spaceship";
      });
  CHECK(has_conflicts1);
  auto has_conflicts2 = !collide.define_<any_thing<cref>, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->S:" + r.name();
      });
  CHECK(!has_conflicts2);
  collide.define<any_thing<cref>, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->A:" + r.name();
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid->Spaceship");
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
      [](auto const& any_, [[maybe_unused]] auto const& s) {
        return type_of(any_).name() + "->s"s;
      });
  collide.define<any_thing<cref>, Asteroid>(
      []([[maybe_unused]] auto const& any_, [[maybe_unused]] auto const& a) {
        return "any_->a";
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship).contains("Asteroid"));
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship).contains("Spaceship"));
  CHECK(collide(thing_spaceship, thing_asteroid) == "any_->a");
}

namespace {
struct BlackHole {
  static std::string name() { return "BlackHole"; }
};
}  // namespace

//TEST_CASE("multi_dispatch 5 ambiguous") {
//  dispatch<std::string(virtual_<any_thing<cref>>, virtual_<any_thing<cref>>)>
//      collide;
//
//  // collide.define<Spaceship, any_thing<cref>>([](auto const&, auto const&
//  // any_) {
//  //   return "s->"s + any_.name();
//  // });
//  //  rule #1
//  collide.define<Asteroid, any_thing<cref>>([](auto const&, auto const& any_) {
//    return "a->"s + "*->"s + any_.name();
//  });
//  // rule #2
//  collide.define<any_thing<cref>, Spaceship>([](auto const& any_, auto const&) {
//    return "*->"s + any_.name() + "->s"s;
//  });
//  // rule #3
//  collide.define<any_thing<cref>, Asteroid>([](auto const& any_, auto const&) {
//    return "*->"s + any_.name() + "->a"s;
//  });
//  // rule #4
//  collide.define<any_thing<cref>, BlackHole>([](auto const& any_, auto const&) {
//    return "*->"s + any_.name() + "->bh"s;
//  });
//
//  Asteroid asteroid;
//  Spaceship spaceship;
//  BlackHole blackhole;
//
//  any_thing<cref> thing_asteroid{asteroid}, thing_spaceship{spaceship},
//      thing_blackhole{blackhole};
//
//  // conflict
//  CHECK(collide(thing_asteroid, thing_spaceship) == "a->*->Spaceship");  // #1
//  CHECK(collide(thing_asteroid, thing_spaceship) == "*->Asteroid->s");   // #2
//
//  CHECK(collide(thing_spaceship, thing_spaceship) == "*->Spaceship->s");  // #2
//
//  // conflict
//  CHECK(collide(thing_asteroid, thing_asteroid) == "a->*->Asteroid");  // #1
//  CHECK(collide(thing_asteroid, thing_asteroid) == "*->Asteroid->a");  // #3
//
//  CHECK(collide(thing_spaceship, thing_asteroid) == "*->Spaceship->a");  // #3
//
//  // conflict:
//  CHECK(collide(thing_asteroid, thing_blackhole) == "a->*->BlackHole");  // #1
//  CHECK(collide(thing_asteroid, thing_blackhole) == "*->Asteroid->bh");  // #4
//}

}  // namespace

// some compile time self tests

namespace {
TRAIT(Dummy, anyxx::dynamic_copyable, )

static_assert(!is_open_dispatch_v_table<Dummy<>::v_table_t>);
static_assert(is_open_dispatch_v_table<thing<>::v_table_t>);

}  // namespace
