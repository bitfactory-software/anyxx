#include <anyxx/anyxx.hpp>
#include <format>
#include <generator>
#include <unordered_map>
#include <vector>

namespace enclosure {

ANY(any_object, )

template <typename Tag>
class enclosure {
 public:
  using id_t = uint64_t;
  static inline constexpr const int no_id = -1;
  class lockable;

  enclosure(enclosure const&) = delete;
  enclosure& operator=(enclosure const&) = delete;

  enclosure() { instance_ = this; }
  ~enclosure() { instance_ = nullptr; }
  static enclosure& instance() { return *instance_; }

  class lockable {
    friend class locked_count;
    friend class updateable;
    friend class enclosure;

   public:
    lockable(any_object<anyxx::shared_const> o) { set_object(std::move(o)); };
    ~lockable() { delete object_.load(); }

    any_object<anyxx::shared_const> get_object() const {
      return *object_.load();
    }
    void set_object(any_object<anyxx::shared_const> o) {
      delete object_.exchange(
          new any_object<anyxx::shared_const>(std::move(o)));
    }

   private:
    lockable(lockable const&) = delete;
    lockable& operator=(lockable const&) = delete;
    lockable(lockable&&) = default;
    lockable& operator=(lockable&&) = default;

    std::atomic<any_object<anyxx::shared_const>*> object_ = nullptr;
    std::atomic_int lock_count = 0;
  };

  class locked_count {
   public:
    locked_count(lockable& lockable) : locked(&lockable) {
      locked->lock_count++;
    }
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
               any_object<anyxx::shared_const> const& o)
        : locked_count_(std::move(locked_count)),
          id(id_),
          object(*anyxx::clone_to<any_object<anyxx::unique>>(o)) {}
    const id_t id;
    any_object<anyxx::unique> object;

   private:
    locked_count locked_count_;
  };

  class transaction {
    friend class enclosure;
    std::vector<updateable> updates_;

   public:
    void checkin(updateable update) {
      updates_.emplace_back(std::move(update));
    };
  };

  class commited_transaction {
    friend class enclosure;
    std::unordered_map<id_t, any_object<anyxx::unique>> updates_;
    std::shared_ptr<commited_transaction> next_;
  };

  template <template <anyxx::is_erased_data> typename ToAny>
  class pointer {
    friend class enclosure;
    pointer(id_t id, lockable* lockable) noexcept {
      this->id_ = id;
      resolved_ = lockable;
    }

   public:
    pointer() = default;
    pointer(pointer const& r) noexcept
        : id_(r.id_), resolved_(r.resolved_.load()) {}
    pointer& operator=(pointer r) noexcept {
      swap(*this, r);
      return *this;
    }

    friend void swap(pointer& l, pointer& r) noexcept {
      using std::swap;
      swap(l.id_, r.id_);
      auto l_resolved = l.resolved_.load();
      auto r_resolved = r.resolved_.exchange(l_resolved);
      l.resolved_.exchange(r_resolved);
    }

    id_t id() const { return id_; }
    explicit operator bool() const { return dereference(); }
    ToAny<anyxx::shared_const> operator->() const { return dereference(); }
    ToAny<anyxx::shared_const> operator*() const { return dereference(); }
    ToAny<anyxx::shared_const> dereference() const {
      if (id_ == no_id) return {};
      if (resolved_) return *load();
      auto dereferenced = enclosure::instance().dereference(id_);
      resolved_.exchange(dereferenced);
      return *load();
    }

   private:
    auto load() const {
      return borrow_as<ToAny<anyxx::shared_const>>(
          resolved_.load()->get_object());
    }
    id_t id_ = no_id;
    mutable std::atomic<lockable*> resolved_ = nullptr;
  };

  template <template <anyxx::is_any> typename AnyObject, typename T,
            typename... Args>
  id_t insert(Args&&... args) {
    return insert(AnyObject<anyxx::shared_const>{std::in_place_type<T>,
                                                 std::forward<Args>(args)...});
  }
  id_t insert(any_object<anyxx::shared_const> o) {
    auto id = next_id++;
    table_[id] = std::make_unique<lockable>(std::move(o));
    return id;
  }

  template <template <anyxx::is_erased_data> typename AnyObject>
  std::optional<AnyObject<anyxx::shared_const>> dereference_as(id_t id) const {
    if (auto found = dereference(id))
      if (auto o =
              borrow_as<AnyObject<anyxx::shared_const>>(found->get_object()))
        return *o;
    return {};
  }

  inline static auto match_all = [](auto const& o) { return true; };

  template <template <anyxx::is_erased_data> typename AnyObject, typename Query,
            typename... Args>
  std::generator<pointer<AnyObject> const&> find(Query const& match,
                                                 Args&&... args) const {
    for (auto const& [id, holder] : table_)
      if (auto o =
              borrow_as<AnyObject<anyxx::shared_const>>(holder->get_object()))
        if (match(o, std::forward<Args>(args)...)) co_yield {id, holder.get()};
  }

  template <template <anyxx::is_erased_data> typename AnyObject, typename Query,
            typename... Args>
  std::optional<pointer<AnyObject>> find_front(Query const& match,
                                               Args&&... args) const {
    for (auto const& pointer :
         find<AnyObject>(match, std::forward<Args>(args)...))
      return pointer;
    return {};
  }

  template <typename V, template <anyxx::is_erased_data> typename AnyObject,
            typename Query, typename... Args>
  std::generator<
      pointer<typename anyxx::bound_typed_any<V, AnyObject>::type> const&>
  find(Query const& match, Args&&... args) const {
    for (auto const& pointer :
         find<AnyObject>(match, std::forward<Args>(args)...))
      if (auto typed = unerase_cast_if<V>(*pointer))
        co_yield {pointer.id_, pointer.resolved_.load()};
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

  void update(auto body) {
    transaction t;
    if (!body(t)) return;
    for (auto&& u : t.updates_) checkin(std::move(u));
  }

 private:
  static enclosure* instance_;
  std::unordered_map<id_t, std::unique_ptr<lockable>> table_;
  id_t next_id = 0;

  lockable* dereference(id_t id) const {
    if (auto found = table_.find(id); found != table_.end())
      return found->second.get();
    return {};
  }

  void checkin(updateable updated) {
    table_[updated.id]->set_object(
        move_to<any_object<anyxx::shared_const>>(std::move(updated.object)));
  }
};

}  // namespace enclosure
