#include <bit_factory/anyxx.hpp>

namespace test::component_base {

ANY_TEMPLATE(((KEY), (VALUE)), any_map,
             (ANY_METHOD(VALUE const&, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE_(((KEY), (VALUE)), any_mutable_map, any_map, (KEY, VALUE),
              (ANY_METHOD_OVERLOAD(VALUE&, at, (KEY), ),
               ANY_OP(VALUE&, [], (KEY), )))

ANY_TEMPLATE(((KEY), (VALUE)), any_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE(((KEY), (VALUE)), any_mutable_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), ),
              ANY_METHOD(std::size_t, size, (), const)))

}