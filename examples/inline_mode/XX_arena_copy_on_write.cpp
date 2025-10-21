#include <anyxx/anyxx.hpp>
#include <arena/arena.hpp>
#include <catch.hpp>
#include <chrono>
#include <cmath>
#include <generator>
#include <iostream>
#include <mutex>
#include <print>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace Catch::Matchers;

namespace example_app {
struct example_tag;
using arena = ::arena::arena<example_tag>;
}  // namespace example_app

namespace arena {
::arena::arena<example_app::example_tag>* ::arena::arena<
    example_app::example_tag>::instance_ = nullptr;
}

namespace example_app {
template <template <anyxx::is_erased_data> typename ToAny>
using pointer = arena::pointer<ToAny>;

ANY_(any_named, ::arena::any_object, (ANY_CONST_METHOD(std::string, get_name)))

auto match_name = [](auto const& o, std::string_view query) {
  return o->get_name() == query;
};

struct named {
  named(std::string s) : name(std::move(s)) {}
  std::string name;
  std::string get_name() const { return name; };
};

struct role : named {
  using named::named;
};
ANY_REGISTER_MODEL(role, any_named);

struct person : named {
  using named::named;
  pointer<anyxx::bound_typed_any<example_app::role, any_named>::type> role;
  double salary = 1000;
};
ANY_REGISTER_MODEL(person, any_named);

}  // namespace example_app

TEST_CASE("example XX/ arena copy on write") {
  using namespace example_app;

  example_app::arena db;

  auto id_miller = db.insert<any_named, person>("Miller");
  CHECK(id_miller == 0);
  auto id_johnson = db.insert<any_named, person>("Johnson");
  CHECK(id_johnson == 1);

  auto id_programmer = db.insert<::arena::any_object, role>("Programmer");
  CHECK(id_programmer == 2);
  auto id_manager = db.insert<any_named, role>("Manager");
  CHECK(id_manager == 3);

  CHECK(unerase_cast<person>(*db.dereference_as<any_named>(id_miller))->name ==
        "Miller");
  CHECK(db.dereference_as<any_named>(id_johnson)->get_name() == "Johnson");
  CHECK(
      unerase_cast<role>(*db.dereference_as<any_named>(id_programmer))->name ==
      "Programmer");
  CHECK(unerase_cast<role>(*db.dereference_as<any_named>(id_manager))->name ==
        "Manager");

  for (auto found : db.find<any_named>(example_app::arena::match_all)) {
    std::println("{}: {}", found.id(), found->get_name());
  }

  {
    bool found_one = false;
    for (auto found : db.find<any_named>(match_name, "Johnson")) {
      CHECK(found.id() == id_johnson);
      CHECK(unerase_cast<person>(*found)->name == "Johnson");
      found_one = true;
    }
    CHECK(found_one);
  }
  {
    bool found_one = false;
    for (auto found : db.find<any_named>(match_name, "Programmer")) {
      CHECK(found.id() == id_programmer);
      CHECK(unerase_cast<role>(*found)->name == "Programmer");
      found_one = true;
    }
    CHECK(found_one);
  }

  {
    db.update([&](auto& transaction) {
      auto update = db.checkout(id_johnson);
      CHECK(update);
      CHECK(!db.checkout(id_johnson));
      transaction.checkin(std::move(*update));
      return true;
    });
    {
      db.update([&](auto& transaction) {
        auto update = db.checkout(id_johnson);
        CHECK(update);
        return false;
      });
    }
    {
      db.update([&](auto& transaction) {
        auto update = db.checkout(id_johnson);
        CHECK(update);
        return true;
      });
    }
    {
      db.update([&](auto& transaction) {
        auto update = db.checkout(id_johnson);
        CHECK(update);
        return false;
      });
    }
  }

  {
    example_app::pointer<anyxx::bound_typed_any<role, any_named>::type> role_pointer{
        *db.find_front<anyxx::bound_typed_any<role, any_named>::type>(match_name,
                                                               "Programmer")};
    auto programmer_any = role_pointer.dereference();
    CHECK(programmer_any->name == "Programmer");
  }

  db.update([&](auto& transaction) {
    return db.checkout(id_johnson)
        .transform([&](example_app::arena::updateable&& update) {
          CHECK(update.id == id_johnson);
          unerase_cast<person>(update.object)->name = "Smith";
          unerase_cast<person>(update.object)->role =
              *db.find_front<anyxx::bound_typed_any<role, any_named>::type>(
                  match_name, "Programmer");
          transaction.checkin(std::move(update));
          return true;
        });
  });
  {
    auto miller = db.dereference_as<any_named>(id_johnson);
    CHECK(miller->get_name() == "Smith");
    auto unerased = unerase_cast<person>(*miller);
    auto role = unerased->role;
    CHECK(role->get_name() == "Programmer");
  }

  for (auto found : db.find<any_named>(example_app::arena::match_all)) {
    if (auto p = unerase_cast_if<person>(*found)) {
      std::println("({}){}: {}", found.id(), p->name, p->salary);
    }
  }

  std::atomic<int> hits = 0;
  std::atomic<int> misses = 0;
  auto salary_lottery = [&](double inc, auto frequence) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, 1);

    for (auto i : std::views::iota(0, 50)) {
      auto person_id = uniform_dist(e1);
      db.update([&](auto& transaction) {
        return db.checkout(person_id)
            .or_else([&] -> std ::optional<example_app::arena::updateable> {
              misses++;
              return {};
            })
            .transform([&](example_app::arena::updateable&& update) {
              unerase_cast<person>(update.object)->salary += inc;
              transaction.checkin(std::move(update));
              hits++;
              return true;
            });
      });
      std::this_thread::sleep_for(frequence);
    }
  };

  using namespace std::chrono_literals;
  std::vector<std::thread> threads;
  threads.emplace_back(salary_lottery, 500, 20ms);
  threads.emplace_back(salary_lottery, -200, 19ms);
  threads.emplace_back(salary_lottery, 50, 10ms);
  threads.emplace_back(salary_lottery, -20, 5ms);
  for (auto& thread : threads) thread.join();
  std::println("update with {} misses and {} hits.", misses.load(),
               hits.load());
  CHECK(misses.load() + hits.load() == 200);
  for (auto found : db.find<person, any_named>(example_app::arena::match_all)) {
    std::println("({}){}: {}", found.id(), found->name, found->salary);
  }
}
