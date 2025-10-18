#include <anyxx/anyxx.hpp>
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

using namespace anyxx;

ANY_HAS_DISPATCH(example_db, any_object)

namespace example_db {

using id_t = uint64_t;
ANY(any_object, )

template <template <is_erased_data> typename Any>
using identified = std::pair<id_t, Any<shared_const>>;

class lockable {
  friend class locked_count;
  friend class updateable;
  friend class running_object_table;

 public:
  lockable(any_object<shared_const> o) { set_object(std::move(o)); };
  ~lockable() { delete object_.load(); }

  any_object<shared_const> get_object() const { return *object_.load(); }
  void set_object(any_object<shared_const> o) {
    delete object_.exchange(new any_object<shared_const>(std::move(o)));
  }

 private:
  lockable(lockable const&) = delete;
  lockable& operator=(lockable const&) = delete;
  lockable(lockable&&) = default;
  lockable& operator=(lockable&&) = default;

  std::atomic<any_object<shared_const>*> object_ = nullptr;
  std::atomic_int lock_count = 0;
};

class locked_count {
 public:
  locked_count(lockable& lockable) : locked(&lockable) { locked->lock_count++; }
  ~locked_count() {
    if (locked) locked->lock_count--;
  }
  locked_count(locked_count const& r) { (*this) = r; }
  locked_count& operator=(locked_count const& r) {
    locked = r.locked;
    locked->lock_count++;
    return *this;
  }
  locked_count(locked_count&& r) { (*this) = std::move(r); }
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

static inline constexpr const int no_id = -1;

class running_object_table {
  std::unordered_map<id_t, std::unique_ptr<lockable>> table_;
  id_t next_id = 0;

  lockable* dereference(id_t id) const {
    if (auto found = table_.find(id); found != table_.end())
      return found->second.get();
    return {};
  }

  template <template <is_erased_data> typename ToAny,
            auto GetRunningObjectTable>
  friend class pointer;

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

  template <template <is_erased_data> typename AnyObject>
  std::optional<AnyObject<shared_const>> dereference_as(id_t id) const {
    if (auto found = dereference(id))
      if (auto o = borrow_as<AnyObject<shared_const>>(found->get_object()))
        return *o;
    return {};
  }

  template <template <is_erased_data> typename AnyObject, typename Query,
            typename... Args>
  std::generator<identified<AnyObject> const&> find(Query const& match,
                                                    Args&&... args) const {
    for (auto const& [id, holder] : table_)
      if (auto o = borrow_as<AnyObject<shared_const>>(holder->get_object()))
        if (match(o, std::forward<Args>(args)...)) co_yield {id, *o};
  }

  std::optional<updateable> checkout(id_t id) {
    if (auto found = table_.find(id); found != table_.end())
      if (locked_count locked{*found->second}; found->second->lock_count == 1)
        return std::optional<updateable>{std::in_place, found->first,
                                         std::move(locked),
                                         found->second->get_object()};
      else
        return {};

    throw std::runtime_error(std::format("id {} not found", id));
  }

  void checkin(updateable updated) {
    table_[updated.id]->set_object(
        move_to<any_object<shared_const>>(std::move(updated.object)));
  }
};

template <template <is_erased_data> typename ToAny, auto GetRunningObjectTable>
class pointer {
 public:
  pointer() = default;

  pointer(id_t id) { (*this) = id; }
  pointer& operator=(id_t id) {
    this->id_ = id;
    return *this;
  }

  pointer(pointer const& r) : id_(r.id_), resolved_(r.resolved_.load()) {}
  pointer& operator=(pointer& r) {
    swap(*this, r);
    return *this;
  }

  // pointer(pointer&& r) : pointer(r) {}
  // pointer& operator=(pointer&& r) {
  //   *this = r;
  //   return *this;
  // }

  friend void swap(pointer& l, pointer& r) {
    using std::swap;
    swap(l.id, r.id);
    auto l_resolved = l.resolved_.load();
    auto r_resolved = r.resolved_.exchange(l_resolved);
    l.resolved_.exchange(r_resolved);
  }

  ToAny<shared_const> operator->() const { return dereference(); }
  ToAny<shared_const> operator*() const { return dereference(); }
  ToAny<shared_const> dereference() const {
    if (id_ == no_id) return {};
    if (resolved_) return *load();
    auto dereferenced = GetRunningObjectTable().dereference(id_);
    resolved_.exchange(dereferenced);
    return *load();
  }

 private:
  auto load() const {
    return borrow_as<ToAny<shared_const>>(resolved_.load()->get_object());
  }
  id_t id_ = no_id;
  mutable std::atomic<lockable*> resolved_ = nullptr;
};

auto match_all = [](auto const& o) { return true; };

}  // namespace example_db

namespace example_app {

example_db::running_object_table rot;

example_db::running_object_table& GetRunningObjectTable() { return rot; }

template <template <is_erased_data> typename ToAny>
using pointer = example_db::pointer<ToAny, GetRunningObjectTable>;

ANY_(any_named, example_db::any_object,
     (ANY_CONST_METHOD(std::string, get_name)))

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
  pointer<any_named> role;
  double salary = 1000;
};
ANY_REGISTER_MODEL(person, any_named);

}  // namespace example_app

TEST_CASE("example XX/ running object table") {
  using namespace example_db;
  using namespace example_app;

  auto id_miller = rot.insert<any_named, person>("Miller");
  CHECK(id_miller == 0);
  auto id_johnson = rot.insert<any_named, person>("Johnson");
  CHECK(id_johnson == 1);

  auto id_programmer = rot.insert<any_object, role>("Programmer");
  CHECK(id_programmer == 2);
  auto id_manager = rot.insert<any_named, role>("Manager");
  CHECK(id_manager == 3);

  CHECK(unerase_cast<person>(*rot.dereference_as<any_named>(id_miller))->name ==
        "Miller");
  CHECK(rot.dereference_as<any_named>(id_johnson)->get_name() == "Johnson");
  CHECK(
      unerase_cast<role>(*rot.dereference_as<any_named>(id_programmer))->name ==
      "Programmer");
  CHECK(unerase_cast<role>(*rot.dereference_as<any_named>(id_manager))->name ==
        "Manager");

  for (auto found : rot.find<any_named>(match_all)) {
    std::println("{}: {}", found.first, found.second->get_name());
  }

  {
    bool found_one = false;
    for (auto found : rot.find<any_named>(match_name, "Johnson")) {
      CHECK(found.first == id_johnson);
      CHECK(unerase_cast<person>(found.second)->name == "Johnson");
      found_one = true;
    }
    CHECK(found_one);
  }
  {
    bool found_one = false;
    for (auto found : rot.find<any_named>(match_name, "Programmer")) {
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

  {
    example_app::pointer<any_named> role_pointer{id_programmer};
    auto programmer_any = role_pointer.dereference();
    auto programmer = unerase_cast<role>(programmer_any);
    CHECK(programmer->name == "Programmer");
  }

  rot.checkout(id_johnson).transform([&](updateable&& update) {
    CHECK(update.id == id_johnson);
    unerase_cast<person>(update.object)->name = "Smith";
    unerase_cast<person>(update.object)->role = id_programmer;
    rot.checkin(std::move(update));
    return true;
  });
  {
    auto miller = rot.dereference_as<any_named>(id_johnson);
    CHECK(miller->get_name() == "Smith");
    auto unerased = unerase_cast<person>(*miller);
    auto role = unerased->role;
    CHECK(role->get_name() == "Programmer");
  }

  for (auto found : rot.find<any_named>(match_all)) {
    if (auto p = unerase_cast<person>(&found.second)) {
      std::println("({}){}: {}", found.first, p->name, p->salary);
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
      rot.checkout(person_id)
          .or_else([&] -> std ::optional<updateable> {
            misses++;
            return {};
          })
          .transform([&](updateable&& update) {
            unerase_cast<person>(update.object)->salary += inc;
            rot.checkin(std::move(update));
            hits++;
            return true;
          });
      std::this_thread::sleep_for(frequence);
    }
  };

  using namespace std::chrono_literals;
  std::vector<std::thread> v;
  v.emplace_back(salary_lottery, 500, 20ms);
  v.emplace_back(salary_lottery, -200, 19ms);
  v.emplace_back(salary_lottery, 50, 10ms);
  v.emplace_back(salary_lottery, -20, 5ms);
  for (auto& tr : v) tr.join();
  std::println("update with {} misses and {} hits.", misses.load(), hits.load());
  CHECK(misses.load() + hits.load() == 200);
  for (auto found : rot.find<any_named>(match_all)) {
    if (auto p = unerase_cast<person>(&found.second)) {
      std::println("({}){}: {}", found.first, p->name, p->salary);
    }
  }
}
