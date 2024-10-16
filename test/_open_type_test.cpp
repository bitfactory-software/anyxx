#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/open_object/open_object.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;

struct test_object : open_object::members< test_object >{};

struct test_member : open_object::member< test_object, test_member, std::string > {}; 

TEST_CASE("open object") {
}