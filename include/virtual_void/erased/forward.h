#pragma once


namespace virtual_void::erased
{

template<class E >
concept is_erased = requires( E e, int i )
{
	typename E::void_t;
	{ E::is_const } -> std::convertible_to< bool >;
	typename E::make_erased;

	{ E::make_erased()(i) } -> std::derived_from< E >;
	{ e.data() } -> std::convertible_to< const void* >;
};

}

