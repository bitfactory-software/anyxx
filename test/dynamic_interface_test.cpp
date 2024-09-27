#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/dynamic_interface/dynamic_interface.h"
#include "../include/virtual_void/virtual_void.h"

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
            if constexpr( std::is_base_of_v< type, FROM > )
            {
                return from;
            }
            else
            {
                return 	virtual_void::make_shared_const< std::remove_cvref_t< FROM > >( std::forward< FROM >( from ) );
            }
        }
        template< typename TO >
        static auto unerase( type& from )
        {
            return static_cast< const std::remove_cvref_t< TO >* >( from.data() );
        }
    };

};

const double M_PI = 3.14;

struct position {float x, y;};

DECLARE_INTERFACE_EX(virtual_void::shared_const, shape_vv,
    (void, draw, position),
    (int, count_sides),
    (double, area),
    (double, perimeter)
)

DECLARE_INTERFACE_EX(void*, shape,
    (void, draw, position),
    (int, count_sides),
    (double, area),
    (double, perimeter)
)

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

TEST_CASE( "dynamic interface" ) {

    using namespace virtual_void;

    circle c{12.3};
    square s{32};
    rectangle r{12, 9};
    regular_polygon p{4, 32};

    std::cout << "print_shape" << std::endl;

    print_shape(c);
    print_shape(s);
    print_shape(r);
    print_shape(p);

    std::cout << "print_shape_vv" << std::endl;

    //auto sc = make_shared_const<circle>(c);
    //print_shape_vv(sc);
    print_shape_vv(c);
    print_shape_vv(s);
    print_shape_vv(r);
    print_shape_vv(p);
}

