// virtual_void variant of this yomm2 example via virtual_void m_tables
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp


#include <iostream>
#include <string>
#include <any>

#include "include/catch.hpp"

#include "../../include/virtual_void/virtual_void.h"
#include "../../include/virtual_void/utilities/unnamed__.h"

using std::cout;
using std::string;

namespace
{

struct Node {
};

using any = std::any;

struct Plus : Node {
    Plus( any left, any right)
        : left(left), right(right) {
    }

    any left, right;
};

struct Times : Node {
    Times(any left, any right)
        : left(left), right(right) {
    }

    any left, right;
};

struct Integer : Node {
    explicit Integer(int value) : value(value) {
    }
//    ~Integer() { cout << "~Integer()" << "\n"; }
};

// =============================================================================
// add behavior to existing classes, without changing them

}


namespace
{
// -----------------------------------------------------------------------------
// evaluate

auto value = virtual_void::method< int(const void*) >{ tree_domain };

auto __ = value.override_< Plus >( []( auto expr ) {
    return value( expr->left ) + value( expr->right );
});

auto __ = value.override_< Times >( []( auto expr ) {
    return value( expr->left ) * value( expr->right );
});

auto __ = value.override_< Integer >( []( auto expr ) {
    return expr->value;
});

// -----------------------------------------------------------------------------
// render as Forth

auto as_forth = virtual_void::method< string( const void* ) >{ tree_domain };

auto __ = as_forth.override_< Plus >( []( auto expr ) {
    return as_forth( expr->left ) + " " + as_forth( expr->right ) + " +";
});

auto __ = as_forth.override_< Times >( []( auto expr ) {
    return as_forth( expr->left ) + " " + as_forth( expr->right ) + " *";
});

auto __ = as_forth.override_< Integer >( []( auto expr ) {
    return std::to_string(expr->value);
});

// -----------------------------------------------------------------------------
// render as Lisp

auto as_lisp = virtual_void::method< string( const void* ) >{ tree_domain };

auto __ = as_lisp.override_< Plus >( []( auto expr ) {
    return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});

auto __ = as_lisp.override_< Times >( []( auto expr ) {
    return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});

auto __ = as_lisp.override_< Integer >( []( auto expr ) {
    return std::to_string(expr->value);
});

}

// -----------------------------------------------------------------------------

TEST_CASE( "21_Tree_TE_dispach_via_m_table" )
{
    build_m_tables( tree_domain );

    using virtual_void::make_shared_const;

    auto expr = make_shared_const<Times>(
        make_shared_const<Integer>(2),
        make_shared_const<Plus>(make_shared_const<Integer>(3), make_shared_const<Integer>(4)));

    REQUIRE( value(expr) == 14 );
    std::stringstream out;
    out << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr);
    REQUIRE( out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14" );
    std::cout << out.str() << "\n";

    BENCHMARK("21_Tree_TE_dispach_via_m_table value") {
        return value(expr);
    };
    BENCHMARK("21_Tree_TE_dispach_via_m_table as_lisp") {
        return as_lisp(expr);
    };
}
