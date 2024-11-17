#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <catch.hpp>

namespace {

namespace DB {
struct IAny {
  virtual ~IAny() = default;
};
struct IFactory {
  virtual IAny* Construct(const std::string&) const = 0;
  virtual ~IFactory() = default;
};
struct ISink {
  virtual void Recieve(const IAny*) const = 0;
};
using Line = std::pair<std::string, std::string>;
using File = std::vector<Line>;
struct System {
  File file;
  std::map<std::string, std::unique_ptr<IFactory> > factories;
  void Query(const ISink& sink) {
    for (auto [type, data] : file)
      if (std::unique_ptr<IAny> any{factories[type]->Construct(data)})
        sink.Recieve(any.get());
  }
};
}  // namespace DB

namespace Application {
struct Data : DB::IAny {
  virtual std::string ToString() const = 0;
};
struct IntData final : Data {
  int data = 0;
  IntData(int i) : data(i) {}
  std::string ToString() const override {
    return "Int: " + std::to_string(data);
  }
  struct Factory : DB::IFactory {
    DB::IAny* Construct(const std::string& data) const override {
      return new IntData(std::atoi(data.c_str()));
    }
  };
};
struct StringData final : Data {
  std::string data;
  StringData(const std::string& s) : data(s) {}
  std::string ToString() const override { return "String: " + data; }
  struct Factory : DB::IFactory {
    DB::IAny* Construct(const std::string& data) const override {
      return new StringData(data);
    }
  };
};
struct ReportSink : DB::ISink {
  void Recieve(const DB::IAny* any) const override {
    if (auto data = dynamic_cast<const Data*>(any))
      std::cout << data->ToString() << std::endl;
  }
};
}  // namespace Application

TEST_CASE("03_Sink_Inheritance") {
  using namespace Application;
  DB::System db;
  db.file = {{"i", "1"}, {"i", "4711"}, {"s", "hello"}, {"s", "world"}};
  db.factories["i"] = std::make_unique<IntData::Factory>();
  db.factories["s"] = std::make_unique<StringData::Factory>();
  ReportSink sink;
  db.Query(sink);
}

}  // namespace