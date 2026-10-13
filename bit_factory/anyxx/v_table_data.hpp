#pragma once

#include <bit_factory/anyxx/voidness.hpp>
#include <bit_factory/anyxx/errors/type_mismatch_error.hpp>

#include <cassert>
#include <typeinfo>
#include <type_traits>
#include <memory>

namespace anyxx {

using allocate_t = mutable_void (*)();
using copy_constructor_t = mutable_void (*)(mutable_void, const_void);
template <typename VTable>
concept is_copy_constructor_v_table =
    requires(VTable* v_table, mutable_void dummy, mutable_void placement,
             const_void from) {
      { v_table->copy_constructor } -> std::convertible_to<copy_constructor_t>;
    };
using move_constructor_t = mutable_void (*)(mutable_void, mutable_void);
template <typename VTable>
concept is_move_constructor_v_table =
    requires(VTable* v_table, mutable_void placement, mutable_void from) {
      { v_table->move_constructor } -> std::convertible_to<move_constructor_t>;
    };
using destructor_t = void (*)(mutable_void);
template <typename VTable>
concept is_destructor_v_table =
    requires(VTable* v_table, mutable_void dummy, mutable_void placement) {
      { v_table->destructor } -> std::convertible_to<destructor_t>;
    };
using delete_t = void (*)(mutable_void);
template <typename VTable>
concept is_delete_v_table =
    requires(VTable* v_table, mutable_void dummy, mutable_void placement) {
      { v_table->delete_ } -> std::convertible_to<delete_t>;
    };

struct model_size_t {
  std::size_t size;
  bool trivial;
};
template <typename Model>
constexpr inline model_size_t compute_model_size() {
    return { .size = sizeof(Model),
            .trivial = std::is_trivially_default_constructible_v<Model> &&
                       std::is_trivially_copyable_v<Model> };
}
template <typename VTable>
concept is_model_size_v_table = requires(VTable* v_table) {
  { v_table->model_size } -> std::convertible_to<model_size_t>;
};

template <typename VTable>
concept is_type_info_v_table = requires(VTable* v_table) {
  { v_table->type_info_ } -> std::convertible_to<std::type_info const*>;
};

using is_derived_from_t = bool (*)(const std::type_info&);
template <typename VTable>
concept is_dynamic_castable_v_table = requires(VTable* v_table) {
  { v_table->is_derived_from_ } -> std::convertible_to<is_derived_from_t>;
};

class meta_data;
template <typename VTable>
concept is_meta_data_v_table = requires(VTable* v_table) {
  { v_table->meta_data_ } -> std::convertible_to<meta_data*>;
};

template <typename M, typename V>
concept is_op_pre_increment_model_map =
    requires(M model_map, V v) { model_map.op_pre_increment(v); };

template <typename M, typename V>
concept is_op_pre_decrement_model_map =
    requires(M model_map, V v) { model_map.op_pre_decrement(v); };

template <typename VTable>
concept is_op_pre_increment_v_table =
    requires(VTable* v_table, mutable_void x) { v_table->op_pre_increment(x); };

template <typename VTable>
concept is_op_pre_decrement_v_table =
    requires(VTable * v_table, mutable_void x) { v_table->op_pre_decrement(x); };


template <typename VTable>
void set_is_derived_from(auto v_table) {
  if constexpr (anyxx::is_dynamic_castable_v_table<VTable>) {
    v_table->is_derived_from_ = +[](const std::type_info& from) {
      return VTable::static_is_derived_from(from);
    };
  }
}

inline bool is_derived_from(const std::type_info& from,
                            is_dynamic_castable_v_table auto* v_table) {
  return v_table->is_derived_from_(from);
}

inline mutable_void allocate(is_model_size_v_table auto* v_table) {
  return ::operator new(v_table->model_size.size);
}
template <typename T>
inline void deallocate(T v_table, mutable_void data) {
  if (!data) return;
  if constexpr (!std::same_as<T, std::nullptr_t>) {
    assert(v_table);
    ::operator delete(data, v_table->model_size.size);
  }
}

inline model_size_t get_model_size(std::nullptr_t) { return {0, true}; }
inline model_size_t get_model_size(is_model_size_v_table auto* v_table) {
  return v_table ? v_table->model_size : model_size_t{0, true};
}
inline mutable_void copy_construct_at(is_copy_constructor_v_table auto* v_table,
                                      mutable_void placement, const_void from) {
  return v_table->copy_constructor(placement, from);
}
inline mutable_void copy_construct(is_copy_constructor_v_table auto* v_table,
                                   const_void from) {
  return copy_construct_at(v_table, allocate(v_table), from);
}
inline mutable_void move_construct_at(is_move_constructor_v_table auto* v_table,
                                      mutable_void placement,
                                      mutable_void from) {
  return v_table->move_constructor(placement, from);
}
inline mutable_void move_construct(is_move_constructor_v_table auto* v_table,
                                   mutable_void from) {
  return move_construct_at(v_table, allocate(v_table), from);
}
template <typename T>
inline void invoke_delete_(T v_table, mutable_void& data) noexcept {
  if (!data) return;
  if constexpr (!std::same_as<T, std::nullptr_t>) {
    assert(v_table);
    v_table->delete_(data);
    data = nullptr;
  }
}  // NOLINT
template <typename T>
inline void destruct(T v_table, mutable_void data) noexcept {
  if (!data) return;
  if constexpr (!std::same_as<T, std::nullptr_t>) {
    assert(v_table);
    v_table->destructor(data);
  }
}

template <typename U>
bool type_match(is_type_info_v_table auto* v_table) {
    return *v_table->type_info_ == typeid(std::decay_t<U>);
}

template <typename U>
void check_type_match(is_type_info_v_table auto* v_table) {
  if (!type_match<U>(v_table)) throw type_mismatch_error("type mismatch");
}

template <typename Concrete>
mutable_void invoke_move_constructor([[maybe_unused]] mutable_void placement,
    [[maybe_unused]] mutable_void from) {
    static_assert(std::is_move_constructible_v<Concrete>);
    auto typed_placement = static_cast<Concrete*>(placement);
    auto typed_from = static_cast<Concrete*>(from);
    [[maybe_unused]] auto constructed =
        std::construct_at<Concrete>(typed_placement, std::move(*typed_from));
    assert(placement == constructed);
    std::destroy_at(typed_from);
    return placement;
}

template <typename Concrete>
    requires std::copy_constructible<Concrete>
mutable_void invoke_copy_constructor([[maybe_unused]] mutable_void placement,
    [[maybe_unused]] const_void from) {
    return std::construct_at<Concrete>(static_cast<Concrete*>(placement),
        *static_cast<Concrete const*>(from));
}

}  // namespace anyxx