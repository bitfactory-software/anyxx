#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/dynamic_interface/dynamic_interface.h"
#include "../include/virtual_void/virtual_void.h"

using namespace Catch::Matchers;

namespace dynamic_interface
{
    template<>
    struct trait< virtual_void::shared_const >
    {
        using type = virtual_void::shared_const;
        
        using param_t = type&;

        template< typename FROM >
        static type erase( FROM&& from )
        {
            using from_t = std::remove_cvref_t< FROM >;
            if constexpr( std::is_base_of_v< type, from_t > )
            {
                return from;
            }
            else
            {
                return 	virtual_void::make_shared_const< from_t >( std::forward< FROM >( from ) );
            }
        }
        template< typename CONSTRUCTOR_PARAM >
        static auto unerase( type& from )
        {
            using constructor_param_t = std::remove_cvref_t< CONSTRUCTOR_PARAM >;
            if constexpr( std::is_base_of_v< type, constructor_param_t > )
            {
                return static_cast< const constructor_param_t::type * >( from.data() );
            }
            else
            {
                return static_cast< const constructor_param_t* >( from.data() );
            }
        }
    };

};

const double M_PI = 3.14;

struct position {float x, y;};

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

//template< typename... >
//struct bases_;
//
//template< typename BASE >
//struct bases_< BASE >
//{
//};
//
//template
//    < typename FIRST
//    , typename... MORE
//    >
//struct bases_< FIRST, MORE... >
//{
//    
//};


template< template< typename, template< typename > typename > typename... >
struct bases_;

template< template< typename, template< typename > typename > typename BASE >
struct bases_< BASE >
{
    template< typename E > using type = BASE< E, dynamic_interface::base >;
};

template
    < template< typename, template< typename > typename > typename FIRST
    , template< typename, template< typename > typename > typename... MORE
    >
struct bases_< FIRST, MORE... >
{
    template< typename E > using type = FIRST< E, typename bases_< MORE... >::type >;
};
template< template< typename, template< typename > typename > typename... BASES >
using bases = bases_< BASES... >::type;

using shape = shape_d_i< void*, bases< shape_base, shape_base1 > >;
//using shape = shape_d_i< void*, dynamic_interface::derived< shape_base, dynamic_interface::basic< shape_base1 > > >;

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
};

std::string to_string_( const auto& x )
{
    return std::to_string( x.count_sides() );
}

void print_shape(shape s) {
    s.draw({4.0, 5.0});
    std::cout << "Shape Number Of Sides: " << s.count_sides() << std::endl;
    std::cout << "Shape Perimeter: " << s.perimeter() << std::endl;
    std::cout << "Shape Area: " << s.area() << std::endl;
}

void print_shape_vv(shape_vv s) {
    s.draw({4.0, 5.0});
    std::cout << "Shape Number Of Sides: " << s.count_sides() << std::endl;
    std::cout << "Shape Perimeter: " << s.perimeter() << std::endl;
    std::cout << "Shape Area: " << s.area() << std::endl;
}


void ask_name(to_string_vv a)
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

