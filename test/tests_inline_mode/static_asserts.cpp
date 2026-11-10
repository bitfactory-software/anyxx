#include <bit_factory/anyxx.hpp>

namespace anyxx {
static_assert(!borrowable_from<mutref, cref, observeable_v_table>);
static_assert(borrowable_from<mutref, mutref, observeable_v_table>);
static_assert(borrowable_from<mutref, unique, observeable_v_table>);
static_assert(!borrowable_from<mutref, shared, observeable_v_table>);
static_assert(!borrowable_from<mutref, weak, observeable_v_table>);
static_assert(borrowable_from<mutref, val<>, dynamic_copyable_v_table>);

static_assert(borrowable_from<cref, cref, observeable_v_table>);
static_assert(borrowable_from<cref, mutref, observeable_v_table>);
static_assert(borrowable_from<cref, unique, observeable_v_table>);
static_assert(borrowable_from<cref, shared, observeable_v_table>);
static_assert(!borrowable_from<cref, weak, observeable_v_table>);
static_assert(borrowable_from<cref, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<shared, cref, observeable_v_table>);
static_assert(!borrowable_from<shared, mutref, observeable_v_table>);
static_assert(!borrowable_from<shared, unique, observeable_v_table>);
static_assert(borrowable_from<shared, shared, observeable_v_table>);
static_assert(!borrowable_from<shared, weak, observeable_v_table>);
static_assert(!borrowable_from<shared, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<weak, cref, observeable_v_table>);
static_assert(!borrowable_from<weak, mutref, observeable_v_table>);
static_assert(!borrowable_from<weak, unique, observeable_v_table>);
static_assert(borrowable_from<weak, shared, observeable_v_table>);
static_assert(borrowable_from<weak, weak, observeable_v_table>);
static_assert(!borrowable_from<weak, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<unique, cref, observeable_v_table>);
static_assert(!borrowable_from<unique, mutref, observeable_v_table>);
static_assert(!borrowable_from<unique, unique, observeable_v_table>);
static_assert(!borrowable_from<unique, shared, observeable_v_table>);
static_assert(!borrowable_from<unique, weak, observeable_v_table>);
static_assert(!borrowable_from<unique, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<val<>, cref, observeable_v_table>);
static_assert(!borrowable_from<val<>, mutref, observeable_v_table>);
static_assert(!borrowable_from<val<>, unique, observeable_v_table>);
static_assert(!borrowable_from<val<>, shared, observeable_v_table>);
static_assert(!borrowable_from<val<>, weak, observeable_v_table>);
static_assert(!borrowable_from<val<>, val<>, dynamic_copyable_v_table>);

static_assert(!cloneable_to<mutref>);
static_assert(!cloneable_to<cref>);
static_assert(cloneable_to<shared>);
static_assert(!cloneable_to<weak>);
static_assert(cloneable_to<unique>);
static_assert(cloneable_to<val<>>);

static_assert(!moveable_from<mutref, cref>);
static_assert(moveable_from<mutref, mutref>);
static_assert(!moveable_from<mutref, unique>);
static_assert(!moveable_from<mutref, shared>);
static_assert(!moveable_from<mutref, weak>);
static_assert(!moveable_from<mutref, val<>>);

static_assert(moveable_from<cref, cref>);
static_assert(moveable_from<cref, mutref>);
static_assert(!moveable_from<cref, unique>);
static_assert(!moveable_from<cref, shared>);
static_assert(!moveable_from<cref, weak>);
static_assert(!moveable_from<cref, val<>>);

static_assert(!moveable_from<shared, cref>);
static_assert(!moveable_from<shared, mutref>);
static_assert(moveable_from<shared, unique>);
static_assert(moveable_from<shared, shared>);
static_assert(!moveable_from<shared, weak>);
static_assert(!moveable_from<shared, val<>>);

static_assert(!moveable_from<weak, cref>);
static_assert(!moveable_from<weak, mutref>);
static_assert(!moveable_from<weak, unique>);
static_assert(moveable_from<weak, shared>);
static_assert(moveable_from<weak, weak>);
static_assert(!moveable_from<weak, val<>>);

static_assert(!moveable_from<unique, cref>);
static_assert(!moveable_from<unique, mutref>);
static_assert(moveable_from<unique, unique>);
static_assert(!moveable_from<unique, shared>);
static_assert(!moveable_from<unique, weak>);
static_assert(!moveable_from<unique, val<>>);

static_assert(!moveable_from<val<>, cref>);
static_assert(!moveable_from<val<>, mutref>);
static_assert(!moveable_from<val<>, unique>);
static_assert(!moveable_from<val<>, shared>);
static_assert(!moveable_from<val<>, weak>);
static_assert(moveable_from<val<>, val<>>);

static_assert(is_model_size_v_table<dynamic_copyable_v_table>);
static_assert(is_copy_constructor_v_table<dynamic_copyable_v_table>);
static_assert(is_move_constructor_v_table<dynamic_copyable_v_table>);
static_assert(is_destructor_v_table<dynamic_copyable_v_table>);
static_assert(is_delete_v_table<dynamic_deletable_v_table>);

}  // namespace anyxx