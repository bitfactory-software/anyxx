#pragma once

#include <stdexcept>

#include "forward.h"

namespace virtual_void
{

class error : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

}