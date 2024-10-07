#pragma once


namespace virtual_void::erased
{

template <class BASE, class DERIVED > concept base_of = std::derived_from< DERIVED, BASE >;

template<class E >
concept is_erased = requires( E e, int i )
{
	typename E::void_t;
	{ E::is_const } -> std::convertible_to< bool >;
	typename E::make_erased;

	{ E::make_erased()(i) } -> base_of< E >;
	{ e.data() } -> std::convertible_to< const void* >;
};

template< is_erased ERASED, typename FROM >
ERASED erase_to( FROM&& from )
{
    if constexpr( is_erased< std::remove_reference_t< FROM > > )
    {
        return from;
    }
    else
    {
        return ERASED::make_erased()( std::forward< FROM >( from ) );
    }
}

template< is_erased ERASED, typename CONSTRUCTED_WITH >
auto unerase( auto from )
{
    using constructed_with_t = std::remove_cvref_t< CONSTRUCTED_WITH >;
    if constexpr( is_erased< constructed_with_t > )
    {
        return static_cast< constructed_with_t::conrete_t * >( from );
    }
    else
    {
        if constexpr( ERASED::is_const )
        {
            return static_cast< constructed_with_t const * >( from );
        }
        else
        {
            return static_cast< constructed_with_t * >( from );
        }
    }
}

}

