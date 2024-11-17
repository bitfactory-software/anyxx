#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <typeindex>
#include <vector>
#include <functional>

#include <catch.hpp>

namespace {

namespace DB {
using FactoryFunction = std::function<std::any(const std::string&)>;
using SinkFunction = std::function<void(const std::any&)>;
using Line = std::pair<std::string, std::string>;
using File = std::vector<Line>;
struct System {
  File file;
  std::map<std::string, FactoryFunction> factories;
  void Query(std::string what, const SinkFunction& sink) {
    for (auto [type, data] : file) sink(factories[type](data));
  }
};
}  // namespace DB

namespace Application {
struct IntData {
  int data = 0;
};
struct StringData {
  std::string data;
};
struct DoubleData {
  double data;
};

using AnyVistorMap =
    std::map<std::type_index, std::function<void(const std::any& any)>>;
AnyVistorMap ReportSink;
void AnyVistor(const AnyVistorMap& vistorMap, const std::any& any) {
  if (auto found = vistorMap.find(any.type()); found == vistorMap.end())
    throw std::runtime_error("no dispatch found");
  else
    found->second(any);
}

}  // namespace Application

TEST_CASE("05_Sink_TypeErased_w_any_dispatch_simple") {
  using namespace Application;
  DB::System db;
  db.factories["i"] = [](const std::string& data) -> std::any {
    return IntData{std::atoi(data.c_str())};
  };
  db.factories["s"] = [](const std::string& data) -> std::any {
    return StringData{data};
  };
  ReportSink[typeid(IntData)] = [](const std::any& any) {
    std::cout << "int: " << any_cast<IntData>(&any)->data << std::endl;
  };
  ReportSink[typeid(StringData)] = [](const std::any& any) {
    std::cout << "string: " << any_cast<StringData>(&any)->data << std::endl;
  };
  db.file = {{"i", "1"}, {"i", "4711"}, {"s", "hello"}, {"s", "world"}};
  db.Query("junk", std::bind( &AnyVistor, ReportSink, std::placeholders::_1 ) );
}

}  // namespace