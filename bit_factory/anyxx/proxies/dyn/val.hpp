#pragma once

#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/dyn/cow.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/small_object_size.hpp>
#include <bit_factory/anyxx/utilities.hpp>
#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

template <bool Trivial, std::size_t SmallObjectSize>
struct local_data : std::array<std::byte, SmallObjectSize> {
  static constexpr inline bool is_trivial = Trivial;
};

/// \brief Proxy to manage the captured object as value with small object
/// optimization
/// * If you forward an object to any constructor, this object will be
/// forwarded to the allocated storage.
/// * To pass an object as second parameter, with the std::in_place tag as
/// first, has the same behavior as above
/// * If you pass as first parameter std::in_place_type<...>, the object will
/// be constructed in place in the allocated memory with the other arguments
/// forwarded
/// \ingroup proxies
template <std::size_t SmallObjectSize = small_object_size,
          typename Nullable = std::false_type>
struct val {
  union data_union {
    data_union(cow::holder_base* cow_holder) : heap(cow{cow_holder}) {}
    template <typename T, typename... Args>
    data_union(std::in_place_type_t<T>, Args&&... args) {
      local = {};
      auto location = static_cast<T*>(static_cast<mutable_void>(local.data()));
      std::construct_at<T>(location, std::forward<Args>(args)...);
    }
    data_union(data_union const& other) noexcept { trivial = other.trivial; }
    ~data_union() {
      // do nothing, the destruction  is done by the proxy_trait<val>::destroy
    }
    cow heap;
    local_data<false, SmallObjectSize> local;
    local_data<true, SmallObjectSize> trivial;
  } data;
  mutable_void ptr_ = nullptr;

  val() : data{nullptr}, ptr_{nullptr} {}
  ~val() {}

  template <typename T, typename... Args>
  val(std::in_place_type_t<T> t, Args&&... args)
    requires(sizeof(T) <= SmallObjectSize)
      : data(t, std::forward<Args>(args)...) {
    ptr_ = data.local.data();
  }
  template <typename T, typename... Args>
  val(std::in_place_type_t<T>, Args&&... args)
    requires(sizeof(T) > SmallObjectSize)
      : data{new cow::holder<T>(std::forward<Args>(args)...)} {
    ptr_ = data.heap.data_ptr();
  }
};

template <std::size_t SmallObjectSize = small_object_size>
using nullable_val = val<SmallObjectSize, std::true_type>;

template <std::size_t SmallObjectSize, typename V>
auto visit_value(auto&& visitor, V&& v, model_size_t size) -> decltype(auto) {
  if (size.size > SmallObjectSize) {
    return std::forward<decltype(visitor)>(visitor)(
        std::forward<V>(v).data.heap);
  } else if (!size.trivial) {
    return std::forward<decltype(visitor)>(visitor)(
        std::forward<V>(v).data.local);
  }
  assert(size.trivial);
  return std::forward<decltype(visitor)>(visitor)(
      std::forward<V>(v).data.trivial);
};

template <typename Nullable, std::size_t SmallObjectSize, typename V2>
auto visit_value(auto&& visitor, val<SmallObjectSize, Nullable>& v1,
                 model_size_t size1, V2&& v2, model_size_t size2)
    -> decltype(auto) {
  if (size1.size > SmallObjectSize) {
    if (size2.size > SmallObjectSize) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.heap, std::forward<V2>(v2).data.heap);
    } else if (!size2.trivial) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.heap, std::forward<V2>(v2).data.local);
    }
    assert(size2.trivial);
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.heap, std::forward<V2>(v2).data.trivial);
  } else if (!size1.trivial) {
    if (size2.size > SmallObjectSize) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.local, std::forward<V2>(v2).data.heap);
    } else if (!size2.trivial) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.local, std::forward<V2>(v2).data.local);
    }
    assert(size2.trivial);
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.local, std::forward<V2>(v2).data.trivial);
  }
  assert(size1.trivial);
  if (size2.size > SmallObjectSize) {
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.trivial, std::forward<V2>(v2).data.heap);
  } else if (!size2.trivial) {
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.trivial, std::forward<V2>(v2).data.local);
  }
  assert(size2.trivial);
  return std::forward<decltype(visitor)>(visitor)(
      v1.data.trivial, std::forward<V2>(v2).data.trivial);
};

template <typename Nullable, std::size_t SmallObjectSize>
struct proxy_trait<val<SmallObjectSize, Nullable>>
    : basic_proxy_trait<val<SmallObjectSize, Nullable>> {
  using void_t = void*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = true;
  static constexpr bool allow_any_default_constructibile = Nullable::value;

  template <typename VTable>
  static constexpr bool is_compatible_with_v_table() {
    return is_copy_constructor_v_table<VTable> &&
           is_move_constructor_v_table<VTable> && is_destructor_v_table<VTable>;
  }

  static auto clone_from([[maybe_unused]] mutable_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    assert(v_table);
    val<SmallObjectSize, Nullable> v;
    v.ptr_ = visit_value<SmallObjectSize>(
        overloads{
            [&](cow& heap) {
              heap.emplace(proxy_trait<cow>::clone_from(data_ptr, v_table));
              return heap.data_ptr();
            },
            [&]<bool Trivial>(local_data<Trivial, SmallObjectSize>& local) {
              auto local_data = static_cast<mutable_void>(local.data());
              copy_construct_at(v_table, local_data, data_ptr);
              return local_data;
            }},
        v, get_model_size(v_table));
    return v;
  }

  static void move_to(val<SmallObjectSize, Nullable>& to,
                      [[maybe_unused]] auto v_table_to,
                      val<SmallObjectSize, Nullable>&& from,
                      [[maybe_unused]] auto* v_table_from) {
    if (v_table_from == nullptr && v_table_to == nullptr) return;
    to.ptr_ = visit_value(
        overloads{
            [&](cow& t, cow& f) {
              proxy_trait<cow>::move_to(t, v_table_to, std::move(f),
                                        v_table_from);
              return t.data_ptr();
            },
            [&](cow& t, local_data<false, SmallObjectSize>& f) {
              proxy_trait<cow>::destroy(t, v_table_to);
              return v_table_from->move_constructor(to.data.local.data(),
                                                    f.data());
            },
            [&](cow& t, local_data<true, SmallObjectSize>& f) {
              proxy_trait<cow>::destroy(t, v_table_to);
              to.data.trivial = std::move(f);
              return (mutable_void)&to.data.trivial;
            },
            [&](local_data<false, SmallObjectSize>& t, cow& f) {
              destruct(v_table_to, t.data());
              proxy_trait<cow>::move_to(to.data.heap, nullptr, std::move(f),
                                        nullptr);
              return to.data.heap.data_ptr();
            },
            [&](local_data<false, SmallObjectSize>& t,
                local_data<false, SmallObjectSize>& f) {
              destruct(v_table_to, t.data());
              return v_table_from->move_constructor(to.data.local.data(),
                                                    f.data());
            },
            [&](local_data<false, SmallObjectSize>& t,
                local_data<true, SmallObjectSize>& f) {
              destruct(v_table_to, t.data());
              to.data.trivial = std::move(f);
              return (mutable_void)&to.data.trivial;
            },
            [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t, cow& f) {
              proxy_trait<cow>::move_to(to.data.heap, nullptr, std::move(f),
                                        nullptr);
              return to.data.heap.data_ptr();
            },
            [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t,
                local_data<false, SmallObjectSize>& f) {
              return v_table_from->move_constructor(to.data.local.data(),
                                                    f.data());
            },
            [&](local_data<true, SmallObjectSize>& t,
                local_data<true, SmallObjectSize>& f) {
              t = std::move(f);
              return (mutable_void)&t;
            }},
        to, get_model_size(v_table_to), from, get_model_size(v_table_from));
    from.ptr_ = nullptr;
  }
  // TODO implement move from unique
  // static void move_to(unique& to, auto* to_v_table, val<>&& v,
  //                    auto* v_table) {
  //  assert(v_table);
  //  auto data_ptr =
  //      visit_value(overloads{[&](heap_data& heap) { return heap.release();
  //      },
  //                            [&]<bool Trivial>(local_data<Trivial>& local)
  //                            {
  //                              return move_construct(v_table,
  //                              local.data());
  //                            }},
  //                  v, get_model_size(v_table));
  //  proxy_trait<unique>::move_to(to, to_v_table, unique{data_ptr}, v_table);
  //}

  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return is_copy_constructor_v_table<VTable>;
  }
  static void copy_construct_from(val<SmallObjectSize, Nullable>& to,
                                  auto to_v_table,
                                  val<SmallObjectSize, Nullable> const& from,
                                  auto* from_v_table) {
    if (!from_v_table) return;
    to.ptr_ = visit_value(
        overloads{[&](cow& t, cow const& from_data) {
                    proxy_trait<cow>::copy_construct_from(
                        t, to_v_table, from_data, from_v_table);
                    return t.data_ptr();
                  },
                  [&](cow& t, local_data<false, SmallObjectSize> const& f) {
                    proxy_trait<cow>::destroy(t, to_v_table);
                    return from_v_table->copy_constructor(to.data.local.data(),
                                                          f.data());
                  },
                  [&](cow& t, local_data<true, SmallObjectSize> const& f) {
                    proxy_trait<cow>::destroy(t, to_v_table);
                    to.data.trivial = f;
                    return (mutable_void)&to.data.trivial;
                  },
                  [&](local_data<false, SmallObjectSize>& t, cow const& f) {
                    destruct(to_v_table, t.data());
                    proxy_trait<cow>::assign(to.data.heap, f);
                    return to.data.heap.data_ptr();
                  },
                  [&](local_data<false, SmallObjectSize>& t,
                      local_data<false, SmallObjectSize> const& f) {
                    destruct(to_v_table, t.data());
                    return from_v_table->copy_constructor(to.data.local.data(),
                                                          f.data());
                  },
                  [&](local_data<false, SmallObjectSize>& t,
                      local_data<true, SmallObjectSize> const& f) {
                    destruct(to_v_table, t.data());
                    to.data.trivial = f;
                    return (mutable_void)&to.data.trivial;
                  },
                  [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t,
                      cow const& f) {
                    proxy_trait<cow>::assign(to.data.heap, f);
                    return to.data.heap.data_ptr();
                  },
                  [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t,
                      local_data<false, SmallObjectSize> const& f) {
                    return from_v_table->copy_constructor(to.data.local.data(),
                                                          f.data());
                  },
                  [&](local_data<true, SmallObjectSize>& t,
                      local_data<true, SmallObjectSize> const& f) {
                    t = f;
                    return (mutable_void)&t;
                  }},
        to, get_model_size(to_v_table), from, get_model_size(from_v_table));
  }

  static void destroy(val<SmallObjectSize, Nullable>& v, auto* v_table) {
    visit_value<SmallObjectSize>(
        overloads{
            [&](cow& heap) {
              if (v_table) proxy_trait<cow>::destroy(heap, v_table);
            },
            [&](local_data<false, SmallObjectSize>& local) {
              if (v_table) destruct(v_table, local.data());
            },
            [&]([[maybe_unused]] local_data<true, SmallObjectSize>& local) {}},
        v, get_model_size(v_table));
  }

  static void* get_proxy_ptr_in(val<SmallObjectSize, Nullable> const& v,
                                [[maybe_unused]] auto* v_table) {
    return v.ptr_;
  }
  static void* get_proxy_ptr_in(val<SmallObjectSize, Nullable>& v,
                                auto* v_table) {
    return visit_value<SmallObjectSize>(
        overloads{
            [&](cow& heap) {
              return v.ptr_ = proxy_trait<cow>::get_proxy_ptr_in(heap, v_table);
            },
            [&]<bool Trivial>(
                [[maybe_unused]] local_data<Trivial, SmallObjectSize>& local) {
              return v.ptr_;
            }},
        v, get_model_size(v_table));
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return val<SmallObjectSize, Nullable>(std::in_place_type<std::decay_t<V>>,
                                          std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return val<SmallObjectSize, Nullable>(std::in_place_type<T>,
                                          std::forward<Args>(args)...);
  }
  template <typename ConstructedWith>
  static auto erase(ConstructedWith&& v) {
    return val<SmallObjectSize, Nullable>(
        std::in_place_type<std::decay_t<ConstructedWith>>,
        std::forward<ConstructedWith>(v));
  }
};

static_assert(is_proxy<val<>>);
static_assert(is_object_proxy<val<>>);

}  // namespace anyxx