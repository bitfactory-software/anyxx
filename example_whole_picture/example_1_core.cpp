#include <catch.hpp>
#include <example_whole_picture/layer_0_architecture/architecture.hpp>
#include <example_whole_picture/layer_1_core/shapes/circle/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <iostream>
#include <ranges>

using namespace Catch::Matchers;

using namespace whole_picture;
using namespace whole_picture::architecture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

TEST_CASE("example 1 core") {
  std::cout << whole_picture::core::hello() << "\n";
}

namespace {

struct buffer {
  std::vector<std::vector<char> > buffer_;
  buffer(std::size_t size_x, std::size_t size_y) {
    for (std::size_t line : std::views::iota(0, (int)size_y))
      buffer_.emplace_back(std::vector<char>(size_x, ' '));
  }
  void write(point p, char ch) {
    if (p.x >= 0 && p.x < buffer_[0].size())
      if (p.y >= 0 && p.y < buffer_.size()) {
        buffer_[p.y][p.x] = ch;
      }
  }
  void flush() const {
    for (const auto& line : buffer_) {
      for (const auto& ch : line) std::cout << ch;
      std::cout << "\n";
    }
  }
};

}  // namespace

VV_RUNTIME_STATIC(buffer)
runtime::class_<buffer>::implements<surface> __;

TEST_CASE("example 2 core circle") {
  for (auto r : std::views::iota(1, 7)) {
    buffer b{80, 25};
    auto c = make_circle({12, 12}, r);
    mutable_observed_surface s{b};
    c.draw(s);
    b.flush();
  }
}

TEST_CASE("example 3 core line") {
  {
    buffer b{80, 25};
    auto l = make_line({0, 25}, {80, 0});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    buffer b{80, 25};
    auto l = make_line({0, 0}, {0, 25});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    buffer b{80, 25};
    auto l = make_line({0, 0}, {80, 0});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    buffer b{80, 25};
    auto l = make_line({0, 0}, {80, 25});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
}