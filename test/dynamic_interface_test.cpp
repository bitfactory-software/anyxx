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
        static type erase( FROM* from )
        {
            if constexpr( std::is_base_of< type, FROM > )
            {
                return *from;
            }
            else
            {
                return 	virtual_void::typed_shared_const( *from );
            }
        }
        template< typename TO >
        static auto unerase( type& from )
        {
            return from.data();
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

TEST_CASE( "dynamic interface" ) {

    circle c{12.3};
    square s{32};
    rectangle r{12, 9};
    regular_polygon p{4, 32};
    print_shape(c);
    print_shape(s);
    print_shape(r);
    print_shape(p);

	//using entry_t = std::pair< perfect_typeid_hash::type_id, const char* >;
	//using table_t = std::vector<entry_t>;
	//table_t elements ={ { &typeid(int), "int" }, { &typeid(std::string), "std::string" }, { &typeid(entry_t), "entry_t" } }; 

	//auto not_found = "not found";
	//perfect_typeid_hash::index_table< const char* > index_table( elements, not_found );

	//REQUIRE( index_table[ &typeid(float) ] == not_found );

	//for( auto element : elements )
	//	REQUIRE( index_table[ element.first ] == element.second );
}

