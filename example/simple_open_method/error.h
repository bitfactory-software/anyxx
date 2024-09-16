#pragma once

#include <map>
#include <typeindex>
#include <stdexcept>
#include <functional>

namespace BitFactory::simple_open_method
{
	class error : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};
}