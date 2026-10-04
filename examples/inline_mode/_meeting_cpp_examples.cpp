#define _USE_MATH_DEFINES
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <print>
#include <cmath>

using namespace anyxx;

using namespace anyxx;
TRAIT_(Shape, anyxx::dynamic_deletable,
    (ANY_FN(double, area, (), const)))

    struct Circle { double radius; };
template<> struct Shape_model_map<Circle> {
    double area(const Circle& self) {
        return M_PI * self.radius * self.radius;
    }
};

TEST_CASE("ShapesAny++23") {
    std::initializer_list<any<Shape, unique>> shapes = {Circle{5.0}};
    for(const auto& shape : shapes) {
        std::println("Area: {}", shape.area());;
    }
}
