#pragma once

#include <typeinfo>


namespace anyxx {

struct cast_error {
	std::type_info const& to, & from;
};

}  // namespace anyxx