#pragma once

//#include "virtual_void/class_hierarchy/class_hierarchy.hpp"
import virtual_void;

namespace TestDomain {
struct A1 {
  std::string data;
  A1() = default;
  A1(const std::string& s) : data(s){};
};
struct A2 {
  std::string data;
  A2() = default;
  A2(const std::string& s) : data(s){};
};
struct B1 : A1 {
  std::string data;
  B1() = default;
  B1(const std::string& s) : data(s){};
};
struct B2 : A2 {
  std::string data;
  B2() = default;
  B2(const std::string& s) : data(s){};
};
struct B3 : A1, A2 {
  std::string data;
  B3() = default;
  B3(const std::string& s) : data(s){};
};
struct C1 : B1 {
  std::string data;
  C1() = default;
  C1(const std::string& s) : data(s){};
};
struct C2 : B2 {
  std::string data;
  C2() = default;
  C2(const std::string& s) : data(s){};
};
struct C3 : B3 {
  std::string data;
  C3() = default;
  C3(const std::string& s) : data(s){};
};
struct C {
  std::string data;
  C() = default;
  C(const std::string& s) : data(s){};
};
struct D : C, C3 {
  std::string data;
  D() = default;
  D(const std::string& s) : data(s){};
};
}  // namespace TestDomain

namespace virtual_void::class_hierarchy {
using namespace TestDomain;
template <>
struct class_<A1> : base {};
template <>
struct class_<A2> : base {};
template <>
struct class_<B1> : bases<A1> {};
template <>
struct class_<B2> : bases<A2> {};
template <>
struct class_<B3> : bases<A1, A2> {};
template <>
struct class_<C1> : bases<B1> {};
template <>
struct class_<C2> : bases<B2> {};
template <>
struct class_<C3> : bases<B3> {};
template <>
struct class_<C> : base {};
template <>
struct class_<D> : bases<C, C3> {};
};  // namespace virtual_void::class_hierarchy
