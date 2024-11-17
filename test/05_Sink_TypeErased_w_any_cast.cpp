#include <any>
#include <iostream>
#include <string>
#include <vector>

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

void ReportSink(const std::any& any) {
  if (auto i = any_cast<IntData>(&any))
    std::cout << "int: " << i->data << std::endl;
  else if (auto s = any_cast<StringData>(&any))
    std::cout << "string: " << s->data << std::endl;
}

}  // namespace Application

TEST_CASE("05_Sink_TypeErased_w_any_cast") {
  using namespace Application;
  DB::System db;
  db.factories["i"] = [](const std::string& data) -> std::any {
    return IntData{std::atoi(data.c_str())};
  };
  db.factories["s"] = [](const std::string& data) -> std::any {
    return StringData{data};
  };
  db.file = {{"i", "1"}, {"i", "4711"}, {"s", "hello"}, {"s", "world"}};
  db.Query("junk", ReportSink);
}

}  // namespace