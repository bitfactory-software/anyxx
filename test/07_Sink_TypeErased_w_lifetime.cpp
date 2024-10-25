#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

#include "../../include/std26/proxy.h"
#include "../../include/virtual_void/erased/data/has_m_table/shared_const.h"
#include "../../include/virtual_void/m_table/open_method.h"
#include "../../include/virtual_void/open_method/algorithm.h"
#include "../../include/virtual_void/typeid_cast/cast.h"
#include "include/catch.hpp"

namespace {

namespace DB {
using FactoryFunction =
    std::function<virtual_void::m_table::shared_const_data_ptr(const std::string&)>;
using SinkFunction =
    std::function<void(const virtual_void::m_table::shared_const&)>;

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

virtual_void::m_table::domain applicationDomain;

auto entityToOut =
    virtual_void::m_table::open_method<void(const void*)>{applicationDomain};
auto toString = virtual_void::m_table::open_method<std::string(const void*)>{
    applicationDomain};
auto typeid_const_cast = virtual_void::typeid_cast::const_cast_method<
    virtual_void::m_table::open_method>{applicationDomain};

void IntToOut(const IntData* i) {
  std::cout << "int: " << i->data << std::endl;
}

void AnywhereInTheApplication() { entityToOut.define<IntData>(&IntToOut); }
}  // namespace Application

}  // namespace
namespace virtual_void::class_hierarchy {
using namespace Application;
template <>
struct class_<StringData> : base {};
template <>
struct class_<SuperStringData> : bases<StringData> {};
template <>
struct class_<IntData> : base {};
template <>
struct class_<DoubleData> : base {};
}  // namespace virtual_void::class_hierarchy

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
  virtual_void::m_table::declare_classes(classes{}, applicationDomain);
  virtual_void::m_table::fix_m_tables(applicationDomain);

  db.factories["i"] = [](const std::string& data) {
    return virtual_void::m_table::make_shared_const<IntData>(
        std::atoi(data.c_str()));
  };
  db.factories["s"] = [](const std::string& data) {
    return virtual_void::m_table::make_shared_const<StringData>(data);
  };
  db.factories["ss"] = [](const std::string& data) {
    return virtual_void::m_table::make_shared_const<SuperStringData>(data,
                                                                     "boss");
  };
  db.factories["d"] = [](const std::string& data) {
    return virtual_void::m_table::make_shared_const<DoubleData>(
        std::atof(data.c_str()));
  };

  db.Query("junk", [](const virtual_void::m_table::shared_const& e) {
    // call open method
    std::cout << "type_info: " << e.meta()->type_info()->name() << ": " << toString(e)
              << std::endl;

    try {
      // call open method
      entityToOut(e);
    } catch (std::exception& e) {
      std::cout << "error: " << e.what() << std::endl;
    }

    // cast back from erased -> "unerase"
    if (auto stringData = virtual_void::typeid_cast::cast_to<const StringData>(
            typeid_const_cast, e))
      std::cout << "stringData: " << stringData->data << std::endl;
  });
}

}  // namespace