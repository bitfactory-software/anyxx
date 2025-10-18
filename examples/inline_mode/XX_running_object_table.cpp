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

ANY_HAS_DISPATCH(example_db, any_object)

namespace example_db {

using id_t = uint64_t;
ANY(any_object, )

using identified = std::pair<id_t, any_object<shared_const>>;
class lockable {
  friend class locked_count;
  friend class updateable;
  friend class running_object_table;

 public:
  lockable(any_object<shared_const> o) : object(std::move(o)) {};

 private:
  lockable(lockable const&) = delete;
  lockable& operator=(lockable const&) = delete;
  lockable(lockable&&) = default;
  lockable& operator=(lockable&&) = default;

  any_object<shared_const> object;
  std::atomic_int lock_count = 0;
};

class locked_count {
 public:
  locked_count(lockable& lockable) : locked(&lockable) { locked->lock_count++; }
  ~locked_count() {
    if (locked) locked->lock_count--;
  }
  locked_count(locked_count const& r){
      (*this) = r;
  }
  locked_count& operator=(locked_count const& r) {
    locked = r.locked;
    locked->lock_count++;
    return *this;
  }
  locked_count(locked_count&& r) {
    (*this) = std::move(r);
  }
  locked_count& operator=(locked_count&& r) {
    locked = r.locked;
    r.locked = nullptr;
    return *this;
  }
  friend void swap(locked_count& l, locked_count& r) {
    using std::swap;
    swap(l.locked, r.locked);
  }

 private:
  lockable* locked = nullptr;
};

class updateable {
 public:
  updateable(id_t id_, locked_count&& locked_count,
             any_object<shared_const> const& o)
      : locked_count_(std::move(locked_count)),
        id(id_),
        object(*clone_to<any_object<unique>>(o)) {}
  const id_t id;
  any_object<unique> object;

 private:
  locked_count locked_count_;
};

class running_object_table {
  std::unordered_map<id_t, std::unique_ptr<lockable>> table_;
  id_t next_id = 0;

 public:
  template <template <is_any> typename AnyObject, typename T, typename... Args>
  id_t insert(Args&&... args) {
    return insert(AnyObject<shared_const>{std::in_place_type<T>,
                                          std::forward<Args>(args)...});
  }
  id_t insert(any_object<shared_const> o) {
    auto id = next_id++;
    table_[id] = std::make_unique<lockable>(std::move(o));
    return id;
  }

  std::optional<any_object<shared_const>> dereference(id_t id) const {
    if (auto found = table_.find(id); found != table_.end())
      return found->second->object;
    return {};
  }

  template <template <is_erased_data> typename AnyObject, typename Query,
            typename... Args>
  std::generator<identified const&> find(Query const& match,
                                         Args&&... args) const {
    for (auto const& [id, holder] : table_)
      if (auto o = borrow_as<AnyObject<shared_const>>(holder->object))
        if (match(o, std::forward<Args>(args)...))
          co_yield {id, holder->object};
  }

  std::optional<updateable> checkout(id_t id) {
    if (auto found = table_.find(id); found != table_.end())
      if (locked_count locked{*found->second}; found->second->lock_count == 1)
        return std::optional<updateable>{std::in_place, found->first,
                                         std::move(locked),
                                         found->second->object};
      else
        return {};

    throw std::runtime_error(std::format("id {} not found", id));
  }

  void checkin(updateable updated) {
    table_[updated.id]->object =
        move_to<any_object<shared_const>>(std::move(updated.object));
  }
};

}  // namespace example_db

namespace example_app {

ANY_(any_named, example_db::any_object,
     (ANY_CONST_METHOD(std::string, get_name)))

auto name_match = [](auto const& o, std::string_view query) {
  return o->get_name() == query;
};

struct named {
  named(std::string s) : name(std::move(s)) {}
  std::string name;
  std::string get_name() const { return name; };
};

struct person : named {
  using named::named;
};
// ANY_REGISTER_MODEL(person, any_named);
struct role : named {
  using named::named;
};
ANY_REGISTER_MODEL(role, any_named);

}  // namespace example_app

TEST_CASE("example XX/ running object table") {
  using namespace example_db;
  using namespace example_app;

  running_object_table rot;

  auto id_miller = rot.insert<any_named, person>("Miller");
  CHECK(id_miller == 0);
  auto id_johnson = rot.insert<any_named, person>("Johnson");
  CHECK(id_johnson == 1);

  auto id_programmer = rot.insert<any_object, role>("Programmer");
  CHECK(id_programmer == 2);
  auto id_manager = rot.insert<any_named, role>("Manager");
  CHECK(id_manager == 3);

  CHECK(unerase_cast<person>(*rot.dereference(id_miller))->name == "Miller");
  CHECK(unerase_cast<person>(*rot.dereference(id_johnson))->name == "Johnson");
  CHECK(unerase_cast<role>(*rot.dereference(id_programmer))->name ==
        "Programmer");
  CHECK(unerase_cast<role>(*rot.dereference(id_manager))->name == "Manager");

  {
    bool found_one = false;
    for (auto found : rot.find<any_named>(name_match, "Johnson")) {
      CHECK(found.first == id_johnson);
      CHECK(unerase_cast<person>(found.second)->name == "Johnson");
      found_one = true;
    }
    CHECK(found_one);
  }
  {
    bool found_one = false;
    for (auto found : rot.find<any_named>(name_match, "Programmer")) {
      CHECK(found.first == id_programmer);
      CHECK(unerase_cast<role>(found.second)->name == "Programmer");
      found_one = true;
    }
    CHECK(found_one);
  }

  auto update = rot.checkout(id_johnson);
  CHECK(update);
  CHECK(!rot.checkout(id_johnson));
  rot.checkin(std::move(*update));
  {
    auto update = rot.checkout(id_johnson);
    CHECK(update);
  }
  {
    auto update = rot.checkout(id_johnson);
    CHECK(update);
  }
  rot.checkout(id_johnson).transform([&](updateable&& update) {
    unerase_cast<person>(update.object)->name = "Miller";
    rot.checkin(std::move(update));
    return true;
  });
  CHECK(unerase_cast<person>(*rot.dereference(id_johnson))->name == "Miller");
}
