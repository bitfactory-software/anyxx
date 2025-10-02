#include <iostream>
#include <catch.hpp>

#include "layer_0_architecture/architecture.hpp"
#include "layer_1_core/core.hpp"


using namespace Catch::Matchers;

TEST_CASE("example 1 core") {
	std::cout << whole_picture::core::hello() << "\n";
}
