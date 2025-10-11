﻿#include <iostream>

#include <catch.hpp>

namespace {

namespace DB {
struct IActionParameter {
  virtual std::string Address() const = 0;
};
struct System {
  void Execute(const IActionParameter& param) {
    std::cout << param.Address() << std::endl;
  }
};
}  // namespace DB

namespace Application {
struct ActionParameter : DB::IActionParameter {
  std::string Address() const override { return "hello world"; };
};
}  // namespace Application

TEST_CASE("02_Callback_TypeErased") {
  DB::System db;
  Application::ActionParameter actionParameter;
  db.Execute(actionParameter);
}

}  // namespace