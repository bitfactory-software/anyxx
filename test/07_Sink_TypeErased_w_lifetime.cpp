#include <any>
#include <catch.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

 #include <virtual_void/data/has_m_table/shared_const.hpp>
 #include <virtual_void/open_method/algorithm.hpp>
 #include <virtual_void/open_method/via_m_table/declare.hpp>
 #include <virtual_void/typeid_cast/cast.hpp>

using namespace virtual_void;
using namespace virtual_void::data::has_m_table;

namespace {

namespace DB {
using FactoryFunction = std::function<shared_const(const std::string&)>;
using SinkFunction = std::function<void(const shared_const&)>;

struct System {
  std::map<std::string, FactoryFunction> factories;
  void Query(std::string what, const SinkFunction& sink) {
    auto dataLines = std::vector<std::pair<std::string, std::string> >{
        {"i", "1"},     {"i", "4711"}, {"s", "hello"},
        {"s", "world"}, {"d", "3.14"}, {"ss", "chief"}};
    for (auto [type, data] : dataLines) sink(factories[type](data));
  }
};
}  // namespace DB

namespace Application {
struct IntData {
  int data = 0;
};
struct StringData {
  StringData(const std::string& s1) : data(s1) {}
  std::string data;
};
struct SuperStringData : StringData {
  SuperStringData(const std::string& s1, const std::string& s2)
      : StringData(s1), more(s2) {}
  std::string more;
};
struct DoubleData {
  double data;
};

std::string ToString_(const auto* x) { return std::to_string(x->data); }

std::string ToString_(const StringData* x) { return x->data; }

std::string ToString_(const SuperStringData* x) {
  return x->data + " -> " + x->more;
}

virtual_void::open_method::via_m_table::domain applicationDomain;

auto entityToOut =
    virtual_void::open_method::via_m_table::declare<void(virtual_void::const_)>{
        applicationDomain};
auto toString = virtual_void::open_method::via_m_table::declare<std::string(
    virtual_void::const_)>{applicationDomain};
auto typeid_const_cast = virtual_void::typeid_cast::const_cast_method<
    virtual_void::open_method::via_m_table::declare>{applicationDomain};

void IntToOut(const IntData* i) {
  std::cout << "int: " << i->data << std::endl;
}

void AnywhereInTheApplication() { entityToOut.define<IntData>(&IntToOut); }
}  // namespace Application

}  // namespace
namespace virtual_void::meta {
using namespace Application;
template <>
struct class_<StringData> : base {};
template <>
struct class_<SuperStringData> : bases<StringData> {};
template <>
struct class_<IntData> : base {};
template <>
struct class_<DoubleData> : base {};
}  // namespace virtual_void::meta

namespace {

namespace Application {
using classes =
    virtual_void::type_list<SuperStringData, StringData, IntData, DoubleData>;
}

TEST_CASE("07_Sink_TypeErased_w_lifetime") {
  using namespace Application;
  using namespace DB;

  AnywhereInTheApplication();

  DB::System db;

  entityToOut.define<StringData>([](const StringData* s) {
    std::cout << "string: " << s->data << std::endl;
  });

  virtual_void::open_method::fill_with_overloads(
      classes{}, toString, [](const auto* x) { return ToString_(x); });
  virtual_void::typeid_cast::fill_const_cast_for(classes{}, typeid_const_cast);
  
  db.factories["i"] = [](const std::string& data) {
    return erased_in_place<shared_const, IntData>(std::atoi(data.c_str()));
  };
  db.factories["s"] = [](const std::string& data) {
    return erased_in_place<shared_const, StringData>(data);
  };
  db.factories["ss"] = [](const std::string& data) {
    return erased_in_place<shared_const, SuperStringData>(data, "boss");
  };
  db.factories["d"] = [](const std::string& data) {
    return erased_in_place<shared_const, DoubleData>(std::atof(data.c_str()));
  };

  int ok = 0;
  int fail = 0;
  db.Query("junk", [&](const shared_const& e) {
    // call open method
    std::cout << "type_info: " << get_meta(e)->type_info()->name() << ": ";
    std::cout << toString(e) << std::endl;

    try {
      // call open method
      entityToOut(e);
      ++ok;
    } catch (std::exception& ex) {
      std::cout << "error: " << ex.what() << ": "
                << get_meta(e)->type_info()->name() << std::endl;
      ++fail;
    }

    // cast back from erased -> "unerase"
    if (auto stringData = virtual_void::typeid_cast::cast_to<const StringData>(
            typeid_const_cast, e))
      std::cout << "stringData: " << stringData->data << std::endl;
  });
  REQUIRE(ok == 4);    // String, int
  REQUIRE(fail == 2);  // Double, SuperString
}

}  // namespace