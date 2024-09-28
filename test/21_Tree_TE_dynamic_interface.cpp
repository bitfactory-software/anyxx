//// virtual_void variant of this yomm2 example via c++RTTI
//// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp
//
//#include <iostream>
//#include <iostream>
//#include <string>
//#include <memory>
//
//#include "include/catch.hpp"
//
//#include "../include/dynamic_interface/dynamic_interface.h"
//#include "../include/virtual_void/virtual_void.h"
//
//using std::cout;
//using std::string;
//
//namespace
//{
//
//DECLARE_INTERFACE(node_i,
//    (int, value),
//    (string, as_forth),
//    (string, as_lisp),
//)
//
//using node = node_i< virtual_void::shared_const >;
//
//struct Plus  {
//    Plus( node left, node right)
//        : left(left), right(right) {
//    }
//    int value() const { return left.value() + right.value(); }
//    string as_forth() const { return left->as_forth() + " " + right.as_forth() + " +"; }
//    string as_lisp() const { return "(plus " + left.as_lisp() + " " + right.as_lisp() + ")"; }
//
//    node left, right;
//};
//
//struct Times  {
//    Times(node left, node right)
//        : left(left), right(right) {
//    }
//    int value() const { return left.value() * right.value(); }
//    string as_forth() const { return left.as_forth() + " " + right.as_forth() + " *"; }
//    string as_lisp() const { return "(times " + left.as_lisp() + " " + right.as_lisp() + ")"; }
//
//    node left, right;
//};
//
//struct Integer  {
//    explicit Integer(int value) : int_(value) {
//    }
//    int value() const { return int_; }
//    string as_forth() const { return std::to_string(int_); }
//    string as_lisp() const { return std::to_string(int_); }
//
//    int int_;
//};
//
//}
//
////TEST_CASE( "21_Tree_TE_dynamic_interface" ) 
////{
////    using virtual_void::make_shared_const;
////
////    auto expr = make_shared<Times>(
////        make_shared<Integer>(2),
////        make_shared<Plus>(make_shared<Integer>(3), make_shared<Integer>(4)));
////
////    REQUIRE( expr.value() == 14 );
////    std::stringstream out;
////    out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value();
////    REQUIRE( out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14" );
////    std::cout << out.str() << "\n";
////
////    BENCHMARK("21_Tree_TE_dynamic_interface benchmark") {
////        return expr.value();
////    };
////}
////
