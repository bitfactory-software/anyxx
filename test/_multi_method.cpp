#include <catch.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/multi_method.hpp>
#include <virtual_void/utillities/unnamed__.hpp>
#

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::runtime;
using namespace virtual_void::data;
using namespace virtual_void::interface;

namespace {}

VV_V_TABLE_INSTANCE_ON_THE_FLY(, Thing)
VV_V_TABLE_HAS_EXTENSION_METHODS(, Thing)

namespace {

VV_INTERFACE(Thing, )

class Asteroid {};

class Spaceship {};

};  // namespace

VV_EXTENSION_METHOD_COUNT(Thing)

VV_EXTENSION_TABLE_INSTANCE(Asteroid, Thing)
VV_EXTENSION_TABLE_INSTANCE(Spaceship, Thing)

//multi_method<Thing<const_observer>, Thing<const_observer>,
//             std::string(virtual_void::const_, virtual_void::const_)>
multi_method<
             std::string(virtual_<Thing<const_observer>>, virtual_<Thing<const_observer>>)>
    collide;

namespace {

auto __ =
    collide.define<Asteroid, Spaceship>([](auto a, auto s) { return "a->s"; });
auto __ =
    collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
auto __ =
    collide.define<Spaceship, Spaceship>([](auto a, auto s) { return "s->s"; });
auto __ =
    collide.define<Spaceship, Asteroid>([](auto a, auto s) { return "s->a"; });

TEST_CASE("virtual_typed/interface/multi_method") {
  CHECK(Thing_v_table::imlpementation<Asteroid>()
            ->own_extension_method_holder_t::extension_method_table->size() ==
        2);
  CHECK(Thing_v_table::imlpementation<Spaceship>()
            ->own_extension_method_holder_t::extension_method_table->size() ==
        2);

  Asteroid asteroid;
  Spaceship spaceship;

  Thing<const_observer> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "a->s");
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship) == "s->s");
  CHECK(collide(thing_spaceship, thing_asteroid) == "s->a");
}

}  // namespace

namespace {
template <is_interface INTERFACE>
struct virtual_ {
  using type = INTERFACE;
};

template <typename ARG>
struct translate_erased_function_param {
  using type = ARG;
};
template <is_interface INTERFACE>
struct translate_erased_function_param<virtual_<INTERFACE>> {
  using type = typename INTERFACE::void_t;
};

template <typename RET, typename... ARGS>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<ARGS>::type...);
};

template <std::size_t COUNT, typename... ARGS>
constexpr std::size_t multi_method_dimension_count = COUNT;
template <std::size_t COUNT, is_interface INTERFACE, typename... ARGS>
constexpr std::size_t
    multi_method_dimension_count<COUNT, virtual_<INTERFACE>, ARGS...> =
        multi_method_dimension_count<COUNT + 1, ARGS...>;

template <typename R, typename... CLASSES>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... ARGS>
  struct striped_virtuals {
    static R function(CLASSES*... classes, ARGS... args) {
      return FUNCTOR{}(classes..., args...);
    };
  };

  template <typename FUNCTOR, is_interface INTERFACE, typename... ARGS>
  struct striped_virtuals<FUNCTOR, virtual_<INTERFACE>, ARGS...>
      : striped_virtuals<FUNCTOR, ARGS...> {};

  template <typename... ARGS>
  static auto instance(
      auto functor)  // if functor is a templated operator() from a
                     // stateless function object, instantiate it now!;
  {
    using functor_t = decltype(functor);
    if constexpr (std::is_pointer_v<functor_t>) {
      return functor;
    } else {
      return striped_virtuals<functor_t, ARGS...>::function;
    }
  }
};

template <typename... METHOD_ARGS>
struct args_to_tuple {
  template <typename T, typename... ACTUAL_ARGS>
  auto operator()(T&& dispatch_args, ACTUAL_ARGS&&... args) {
    return std::tuple_cat(dispatch_args,
                          std::make_tuple(std::forward<ACTUAL_ARGS>(args)...));
  }
};
template <is_interface INTERFACE, typename... METHOD_ARGS>
struct args_to_tuple<virtual_<INTERFACE>, METHOD_ARGS...> {
  template <typename T, typename... ACTUAL_ARGS>
  auto operator()(T const& dispatch_args, auto && dispatch_arg,
                  ACTUAL_ARGS&&... args) {
    return args_to_tuple<METHOD_ARGS...>{}(std::tuple_cat(
        dispatch_args, std::make_tuple(get_void_data_ptr(dispatch_arg)),
        std::forward<ACTUAL_ARGS>(args)...));
  }
};

template <typename DISPATCH, typename... ARGS>
struct dispatch_matrix {
  using type = DISPATCH;
};
template <typename DISPATCH, is_interface INTERFACE, typename... ARGS>
struct dispatch_matrix<DISPATCH, virtual_<INTERFACE>, ARGS...>
    : dispatch_matrix<std::vector<DISPATCH>, ARGS...> {};

template <typename R, typename... ARGS>
struct method;
template <typename R, typename... ARGS>
struct method<R(ARGS...)> {
  using erased_function_t =
      typename translate_erased_function<R, ARGS...>::type;

  static constexpr std::size_t dimension_count =
      multi_method_dimension_count<0, ARGS...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, ARGS...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using dispatch_indices = std::array<std::size_t, dimension_count>;

  template <std::size_t DIM, typename... ARGS>
  struct dispatch_access {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename ARGS_TUPLE>
    auto invoke(auto const& target, ARGS_TUPLE&& args, auto&&...) {
      return std::apply(target, std::forward<ARGS_TUPLE>(args));
    }
  };

  template <std::size_t DIM, is_interface INTERFACE, typename... ARGS>
  struct dispatch_access<DIM, virtual_<INTERFACE>, ARGS...>
      : dispatch_access<DIM + 1, ARGS...> {
    using interface_t = INTERFACE;
    using v_table_t = typename interface_t::v_table_t;
    using next_t = dispatch_access<DIM + 1, ARGS...>;

    std::size_t index_ = extension_method_count_of<v_table_t> ++;
    std::size_t dispatch_dimension_size_ = 0;

    template <typename CLASS, typename... CLASSES>
    auto define(auto fp, auto& matrix) {
      auto extension_method_table =
          runtime::extension_method_table_instance<v_table_t, CLASS>();
      auto dispatch_index =
          *runtime::get_multi_method_index_at(extension_method_table, index_)
               .or_else([&] -> std::optional<std::size_t> {
                 runtime::set_multi_method_index_at(
                     extension_method_table, index_, dispatch_dimension_size_);
                 return dispatch_dimension_size_++;
               });
      if (matrix.size() <= dispatch_index) matrix.resize(dispatch_index + 1);
      return next_t::template define<CLASSES...>(fp, matrix[dispatch_index]);
    }

    template <typename ARGS_TUPLE, typename... ACTUAL_ARGS>
    auto invoke(auto const& target, ARGS_TUPLE&& args_tuple,
                INTERFACE&& interface, ACTUAL_ARGS&&... actual_args) const {
      auto extension_method_table =
          get_v_table(interface)->extension_method_table;
      auto dispatch_dim =
          runtime::get_multi_method_index_at(extension_method_table, index_);
      return next_t::invoke(target[dispatch_dim],
                            std::forward<ARGS_TUPLE>(args_tuple),
                            std::forward<ACTUAL_ARGS>(actual_args)...);
    }
  };

  dispatch_access<0, ARGS...> dispatch_access_;

  template <typename FUNCTION, typename... CLASSES>
  auto define(FUNCTION f) {
    auto fp = ensure_function_ptr_from_functor_t<CLASSES..., ARGS...>(f);
    return dispatch_access_.template define<CLASSES>(f, dispatch_matrix_);
  };

  template <typename... ACTUAL_ARGS>
  auto invoke(ACTUAL_ARGS&&... args) {
    auto args_tuple = args_to_tuple<ARGS>{}(std::tuple<>{},
                                            std::forward<ACTUAL_ARGS>(args)...);
    return dispatch_access_.invoke(dispatch_matrix_, args_tuple,
                                   std::forward<ACTUAL_ARGS>(args)...);
  }
};

using example = method<std::string(virtual_<Thing<const_observer>>,
                                   virtual_<Thing<mutable_observer>>, double,
                                   int, Thing<shared_const> const&)>;

static_assert(std::same_as<example::erased_function_t,
                           std::string (*)(void const*, void*, double, int,
                                           Thing<shared_const> const&)>);

static_assert(
    std::same_as<example::dispatch_matrix_t,
                 std::vector<std::vector<example::erased_function_t>>>);

static_assert(example::dimension_count == 2);

auto a_functor = [](auto, auto, double, int, Thing<shared_const> const&) {
  return std::string{};
};

auto a_function = ensure_function_ptr_from_functor_t<std::string, Asteroid,
                                                     Spaceship>::
    instance<virtual_<Thing<const_observer>>, virtual_<Thing<mutable_observer>>,
             double, int, Thing<shared_const> const&>(a_functor);

static_assert(std::same_as<decltype(a_function),
                           std::string (*)(Asteroid*, Spaceship*, double, int,
                                           Thing<shared_const> const&)>);
}  // namespace

VV_V_TABLE_INSTANCE_ON_THE_FLY(, Dummy)
// NOT! VV_V_TABLE_HAS_EXTENSION_METHODS(, Thing)

namespace {
VV_INTERFACE(Dummy, )

template <typename... ARGS>
struct have_extension_methods_enabled {
  static constexpr bool value = true;
};
template <is_interface INTERFACE, typename... ARGS>
  requires has_extension_methods_enabled<INTERFACE>
struct have_extension_methods_enabled<virtual_<INTERFACE>, ARGS...>
    : have_extension_methods_enabled<ARGS...> {};
template <is_interface INTERFACE, typename... ARGS>
  requires(!has_extension_methods_enabled<INTERFACE>)
struct have_extension_methods_enabled<virtual_<INTERFACE>, ARGS...> {
  static constexpr bool value = false;
};
static_assert(!has_extension_methods_enabled<Dummy<const_observer>>);
static_assert(has_extension_methods_enabled<Thing<const_observer>>);

static_assert(
    !have_extension_methods_enabled<virtual_<Dummy<const_observer>>,
                                    virtual_<Dummy<const_observer>>>::value);
static_assert(
    !have_extension_methods_enabled<virtual_<Dummy<const_observer>>,
                                    virtual_<Thing<const_observer>>>::value);
static_assert(
    !have_extension_methods_enabled<virtual_<Thing<const_observer>>,
                                    virtual_<Dummy<const_observer>>>::value);
static_assert(
    have_extension_methods_enabled<virtual_<Thing<const_observer>>,
                                   virtual_<Thing<const_observer>>>::value);

}  // namespace
