#include <anyxx/anyxx.hpp>
#include <catch.hpp>
#include <cmath>
#include <generator>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace Catch::Matchers;

using namespace anyxx;

namespace example {

using id_t = uint64_t;
ANY(any_object, )
ANY(any_query, (ANY_CONST_METHOD(bool, match, any_object<shared_const> const&)))

using identified = std::pair<id_t, any_object<shared_const>>;

class running_object_table {
  std::unordered_map<id_t, any_object<shared_const>> table_;
  id_t next_id = 0;

 public:
  std::optional<any_object<shared_const>> dereference(id_t id) const {
    if (auto found = table_.find(id); found != table_.end())
      return found->second;
    return {};
  }
  void update(id_t id, any_object<shared_const> const& o) { table_[id] = o; }
  id_t insert(any_object<shared_const> const& o) {
    auto id = next_id++;
    table_[id] = o;
    return id;
  }
  std::generator<any_object<shared_const> const&> find(
      any_query<const_observer> const& query) const {
    for (auto const& [id, o] : table_)
      if (query->match(o)) co_yield o;
  }
};

struct named {
  named(std::string s) : name(std::move(s)) {}
  std::string name;
};
struct person : named {
  using named::named;
};

struct role : named {
  using named::named;
};

}  // namespace example

TEST_CASE("example XX/ riunning object table") {
  using namespace example;

  running_object_table rot;

  auto id_miller = rot.insert({std::in_place_type<person>, "Miller"});
  CHECK(id_miller == 0);
  auto id_johnson = rot.insert({std::in_place_type<person>, "Johnson"});
  CHECK(id_johnson == 1);

  auto id_programmer = rot.insert({std::in_place_type<role>, "Programmer"});
  CHECK(id_programmer == 2);
  auto id_manager = rot.insert({std::in_place_type<role>, "Manager"});
  CHECK(id_manager == 3);

  CHECK(unerase_cast<person>(*rot.dereference(id_miller))->name == "Miller");
  CHECK(unerase_cast<person>(*rot.dereference(id_johnson))->name == "Johnson");
  CHECK(unerase_cast<role>(*rot.dereference(id_programmer))->name == "Programmer");
  CHECK(unerase_cast<role>(*rot.dereference(id_manager))->name == "Manager");
}
