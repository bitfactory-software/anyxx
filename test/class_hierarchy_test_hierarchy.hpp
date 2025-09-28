#pragma once

#include <virtual_void/runtime/meta_data.hpp>

namespace TestDomain {
struct A1 {
  std::string data;
  A1() = default;
  A1(const std::string& s) : data(s) {};
};
struct A2 {
  std::string data;
  A2() = default;
  A2(const std::string& s) : data(s) {};
};
struct B1 : A1 {
  std::string data;
  B1() = default;
  B1(const std::string& s) : data(s) {};
};
struct B2 : A2 {
  std::string data;
  B2() = default;
  B2(const std::string& s) : data(s) {};
};
struct B3 : A1, A2 {
  std::string data;
  B3() = default;
  B3(const std::string& s) : data(s) {};
};
struct C1 : B1 {
  std::string data;
  C1() = default;
  C1(const std::string& s) : data(s) {};
};
struct C2 : B2 {
  std::string data;
  C2() = default;
  C2(const std::string& s) : data(s) {};
};
struct C3 : B3 {
  std::string data;
  C3() = default;
  C3(const std::string& s) : data(s) {};
};
struct C {
  std::string data;
  C() = default;
  C(const std::string& s) : data(s) {};
};
struct D : C, C3 {
  std::string data;
  D() = default;
  D(const std::string& s) : data(s) {};
};
}  // namespace TestDomain

VV_RUNTIME(, TestDomain::A1)
VV_RUNTIME(, TestDomain::A2)
VV_RUNTIME(, TestDomain::B1)
VV_RUNTIME(, TestDomain::B2)
VV_RUNTIME(, TestDomain::B3)
VV_RUNTIME(, TestDomain::C1)
VV_RUNTIME(, TestDomain::C2)
VV_RUNTIME(, TestDomain::C3)
VV_RUNTIME(, TestDomain::C)
VV_RUNTIME(, TestDomain::D)
