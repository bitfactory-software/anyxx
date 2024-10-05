// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <iostream>
#include <iostream>
#include <string>
#include <memory>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/interface.h"
//#include "../include/virtual_void/m_table/lifetime.h"
#include "../include/virtual_void/erased/lifetime/shared_const.h"

using std::cout;
using std::string;

namespace
{
using namespace virtual_void::erased;

DECLARE_INTERFACE(node_i,
    (int, value),
    (string, as_forth),
    (string, as_lisp),
)

//using node = node_i< virtual_void::m_table::shared_const >;
using node = node_i< virtual_void::erased::shared_const >;

struct Plus  {
    Plus( node left, node right)
        : left(left), right(right) {
    }
    int value() const { return left.value() + right.value(); }
    string as_forth() const { return left.as_forth() + " " + right.as_forth() + " +"; }
    string as_lisp() const { return "(plus " + left.as_lisp() + " " + right.as_lisp() + ")"; }

    node left, right;
};

struct Times  {
    Times(node left, node right)
        : left(left), right(right) {
    }
    int value() const { return left.value() * right.value(); }
    string as_forth() const { return left.as_forth() + " " + right.as_forth() + " *"; }
    string as_lisp() const { return "(times " + left.as_lisp() + " " + right.as_lisp() + ")"; }

    node left, right;
};

struct Integer  {
    explicit Integer(int value) : int_(value) {
    }
    int value() const { return int_; }
    string as_forth() const { return std::to_string(int_); }
    string as_lisp() const { return std::to_string(int_); }

    int int_;
};

}

TEST_CASE( "21_Tree_TE_dynamic_interface" ) 
{
//    using virtual_void::m_table::make_shared_const;
    using virtual_void::erased::make_shared_const;

    static_assert( is_erased< typed_shared_const< Times > > );
    static_assert( is_erased< typed_shared_const< Plus > > );
    static_assert( is_erased< typed_shared_const< Integer > > );

    auto expr = node( make_shared_const<Times>(
        make_shared_const<Integer>(2),
        make_shared_const<Plus>(make_shared_const<Integer>(3), make_shared_const<Integer>(4))));

    REQUIRE( expr.value() == 14 );
    std::stringstream out;
    out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value();
    REQUIRE( out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14" );
    std::cout << out.str() << "\n";
#ifndef _DEBUG
    BENCHMARK("21_Tree_TE_dynamic_interface value") {
        return expr.value();
    };
    BENCHMARK("21_Tree_TE_dynamic_interface as_lisp") {
        return expr.as_lisp();
    };
#endif // !_DEBUG
}

