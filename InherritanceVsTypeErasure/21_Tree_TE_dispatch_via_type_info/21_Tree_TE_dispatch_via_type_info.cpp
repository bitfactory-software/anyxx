
// virtual_void variant of this yomm2 example
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <iostream>

#include "../../include/virtual_void/virtual_void.h"

#include <iostream>
#include <string>
#include <memory>

using std::cout;
using std::string;

// generate "unused identifier" to simplify overrides outside of function
#define MERGE_(a,b)  a##b
#define LABEL_(a) MERGE_(unique_name_, a)
#define UNIQUE_NAME LABEL_(__LINE__)
#define __ UNIQUE_NAME

struct Node {
    virtual ~Node() = default; // generates c++ vtable + type_info
};

using shared_const_node = std::shared_ptr<const Node>;

struct Plus : Node {
    Plus( shared_const_node left, shared_const_node right)
        : left(left), right(right) {
    }

    shared_const_node left, right;
};

struct Times : Node {
    Times(shared_const_node left, shared_const_node right)
        : left(left), right(right) {
    }

    shared_const_node left, right;
};

struct Integer : Node {
    explicit Integer(int value) : value(value) {
    }
    int value;
};

// =============================================================================
// add behavior to existing classes, without changing them

virtual_void::domain tree_domain;

//+++ no special meta data needed. the dispatch information comes from the typeid() via the c++ vtable
//
//namespace virtual_void::class_hierarchy
//{
//    template<> struct class_< Node > : base {};
//    template<> struct class_< Plus > : bases< Node >{};
//    template<> struct class_< Times > : bases< Node >{};
//    template<> struct class_< Integer > : bases< Node >{};
//
//	auto __ = declare_classes< Node, Plus, Times, Integer >( tree_domain );
//}
//---

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

// -----------------------------------------------------------------------------

int main() {
    //build_v_tables( tree_domain ); no v_tabls, dispatch via typeindex(type_info)->"override_" function

    using std::make_shared;

    auto expr = make_shared<Times>(
        make_shared<Integer>(2),
        make_shared<Plus>(make_shared<Integer>(3), make_shared<Integer>(4)));

    cout << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr)
         << "\n";
    // error_output:
    // 2 3 4 + * = (times 2 (plus 3 4)) = 14

    return 0;
}

