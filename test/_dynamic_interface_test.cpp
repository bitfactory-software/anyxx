#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/dynamic_interface/dynamic_interface.h"
#include "../include/virtual_void/dynamic_interface_traits.h"

using namespace Catch::Matchers;

const double M_PI = 3.14;

struct position {float x, y;};

namespace dynamic_interface
{
    template< typename ERASED, template < typename > typename BASE, typename RET, typename... ARGS >
    struct call_operator_facade : BASE< ERASED >
    {
        using erased_t = ERASED;
        using erased_param_t = trait<ERASED>::param_t;
        using base_t = BASE< ERASED >;
        using base_v_table_t = base_t::_v_table_t;
        struct _v_table_t : base_v_table_t
        {
            RET (*call_op)(erased_param_t, ARGS&&... );
            template <typename _tp>
            _v_table_t(_tp&& param)
                : base_v_table_t( std::forward<_tp>(param) )
                , call_op ( [](erased_param_t _vp, ARGS&&... args )
                    {
                        return ( *trait<erased_t>::unerase<_tp>(_vp) ) ( std::forward< ARGS >(args)...);
                    })
            {}
        } * _v_table;
        template <typename _tp>
        call_operator_facade(_tp&& v) 
            : base_t(std::forward<_tp>(v))
        { 
            static _v_table_t _tp_v_table{ v };
            _v_table = &_tp_v_table;
        }
        RET operator()( ARGS&&... args ) const 
        {
            return _v_table->call_op( base_t::_ref, std::forward< ARGS >(args)...);
        }
        call_operator_facade(const call_operator_facade&) = default;
        call_operator_facade(call_operator_facade&) = default;
        call_operator_facade(call_operator_facade&&) = default;
    };
    template< typename RET, typename... ARGS >
    struct call_operator_
    {
        template< typename ERASED, template < typename > typename BASE >
        using type = call_operator_facade< ERASED, BASE, RET, ARGS... >;
    };
    template< typename RET, typename... ARGS >
    using call_operator = call_operator_< RET, ARGS... >;
}


DECLARE_FREE_INTERFACE(to_string_i,
    (std::string, to_string)
)

DECLARE_INTERFACE(shape_base1,
    (void, draw, position)
)

DECLARE_INTERFACE(shape_base,
    (int, count_sides),
)

DECLARE_INTERFACE(shape_d_i,
    (double, area),
    (double, perimeter)
)

DECLARE_INTERFACE(shape_i,
    (void, draw, position),
    (int, count_sides),
    (double, area),
    (double, perimeter)
)

using to_string_vv = to_string_i< virtual_void::shared_const >;

using shape_vv = shape_i< virtual_void::shared_const, dynamic_interface::base >;

using shape = shape_d_i< void*, dynamic_interface::bases< dynamic_interface::call_operator_< std::string, std::string >::type, shape_base, shape_base1 > >;

struct circle {
    double radius;
    void draw(position p) const {
        std::cout << " A Circle Is Recorded At " << p.x << " " << p.y << std::endl;
    }
    int count_sides() const {
        return 1;
    }
    double area() const {
        return radius * radius * M_PI;
    }
    double circumference() const {
        return radius * 2.0f * M_PI;
    }
    double perimeter() const {
        return circumference();
    }
    std::string operator()( const std::string& x ) { return x + "circle"; }
};
struct square {
    int w;
    void draw(position p) const {
        std::cout << " A Square Is Recorded At " << p.x << " " << p.y << std::endl;
    }
    int count_sides() const {
        return 4;
    }
    double area() const {
        return w * w;
    }
    double perimeter() const {
        return w * 4;
    }
    std::string operator()( const std::string& x ) { return x + "square"; }
};
struct rectangle {
    int w, h;
    void draw(position p) const {
        std::cout << " A Rectangle Is Recorded At " << p.x << " " << p.y << std::endl;
    }
    int count_sides() const {
        return 4;
    }
    double area() const {
        return w * h;
    }
    double perimeter() const {
        return w + w + h + h;
    }
    std::string operator()( const std::string& x ) { return x + "rectangle"; }
};
struct regular_polygon {
    int sides;
    double side_length;
    void draw(position p) const {
        std::cout << " A Polygon Is Recorded At " << p.x << ", " << p.y << std::endl;
    }
    int count_sides() const {
        return sides;
    }
    double apothem() const {
        return (side_length/2) / std::tan(M_PI/sides);
    }
    double radius() const {
        return (side_length/2) / std::sin(M_PI/sides);
    }
    double perimeter() const {
        return sides * side_length;
    }
    double area() const {
        return (perimeter() * apothem()) / 2;
    }
    std::string operator()( const std::string& x ) { return x + "regular_polygon"; }
};

std::string to_string_( auto const x )
{
    return std::to_string( x.count_sides() );
}

void print_shape(const shape s) {
    s.draw({4.0, 5.0});
    std::cout << "Shape Number Of Sides: " << s.count_sides() << std::endl;
    std::cout << "Shape Perimeter: " << s.perimeter() << std::endl;
    std::cout << "Shape Area: " << s.area() << std::endl;
    std::cout << s("Shape type = ") << std::endl;
}

void print_shape_vv(const shape_vv s) {
    s.draw({4.0, 5.0});
    std::cout << "Shape Number Of Sides: " << s.count_sides() << std::endl;
    std::cout << "Shape Perimeter: " << s.perimeter() << std::endl;
    std::cout << "Shape Area: " << s.area() << std::endl;
}


void ask_name(const to_string_vv a)
{
    std::cout << "name: " << a.to_string() << std::endl;
}

TEST_CASE( "dynamic interface" ) {

    using namespace virtual_void;

    circle c{12.3};
    square s{32};
    rectangle r{12, 9};
    regular_polygon p{4, 32};

    std::cout << "print_shape *******************************" << std::endl;

    print_shape(c);
    print_shape(s);
    print_shape(r);
    print_shape(p);

    std::cout << "print_shape_vv ********************************" << std::endl;

    auto sc = make_shared_const<circle>(circle{c});
    static_assert( std::is_same_v< decltype(sc), typed_shared_const< circle > > );
    static_assert( std::is_base_of_v< shared_const, typed_shared_const< circle > > );
    auto& c1 = *sc;
    REQUIRE_THAT( c1.perimeter(),  WithinAbs(77.2, 77.3));
    shape_vv circle_shape_vv = sc;
    auto unerased_circle = as< circle >( *circle_shape_vv.get_erased() );
    REQUIRE_THAT( unerased_circle->perimeter(),  WithinAbs(77.2, 77.3));
    auto x = circle_shape_vv;
    REQUIRE_THAT( circle_shape_vv.perimeter(),  WithinAbs(77.2, 77.3));
    print_shape_vv(sc);
    print_shape_vv(circle_shape_vv);
    print_shape_vv(c);
    print_shape_vv(s);
    print_shape_vv(r);
    print_shape_vv(p);

    std::cout << "ask  ********************************" << std::endl;
    ask_name(sc);
    ask_name(c);
    ask_name(s);
    ask_name(r);
    ask_name(p);
}

