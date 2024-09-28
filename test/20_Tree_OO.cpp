// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <iostream>
#include <iostream>
#include <string>
#include <memory>

#include "include/catch.hpp"

using std::cout;
using std::string;

namespace
{

struct Node {
    virtual ~Node() = default; // generates c++ vtable + type_info
    virtual int value() const = 0;
    virtual string as_forth() const = 0;
    virtual string as_lisp() const = 0;
};

using shared_const_node = std::shared_ptr<const Node>;

struct Plus : Node {
    Plus( shared_const_node left, shared_const_node right)
        : left(left), right(right) {
    }
    int value() const override { return left->value() + right->value(); }
    string as_forth() const override { return left->as_forth() + " " + right->as_forth() + " +"; }
    string as_lisp() const override { return "(plus " + left->as_lisp() + " " + right->as_lisp() + ")"; }

    shared_const_node left, right;
};

struct Times : Node {
    Times(shared_const_node left, shared_const_node right)
        : left(left), right(right) {
    }
    int value() const override { return left->value() * right->value(); }
    string as_forth() const override { return left->as_forth() + " " + right->as_forth() + " *"; }
    string as_lisp() const override { return "(times " + left->as_lisp() + " " + right->as_lisp() + ")"; }

    shared_const_node left, right;
};

struct Integer : Node {
    explicit Integer(int value) : int_(value) {
    }
    int value() const override { return int_; }
    string as_forth() const override { return std::to_string(int_); }
    string as_lisp() const override { return std::to_string(int_); }

    int int_;
};

}

TEST_CASE( "20_Tree_OO" ) 
{
    using std::make_shared;

    auto expr = make_shared<Times>(
        make_shared<Integer>(2),
        make_shared<Plus>(make_shared<Integer>(3), make_shared<Integer>(4)));

    REQUIRE( expr->value() == 14 );
    std::stringstream out;
    out << expr->as_forth() << " = " << expr->as_lisp() << " = " << expr->value();
    REQUIRE( out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14" );
    std::cout << out.str() << "\n";

    BENCHMARK("20_Tree_OO benchmark") {
        return expr->value();
    };
}

