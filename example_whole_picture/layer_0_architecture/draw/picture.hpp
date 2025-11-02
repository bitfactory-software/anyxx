#pragma once

#include <example_whole_picture/layer_0_architecture/architecture.hpp>
#include <ranges>
namespace whole_picture::architecture::draw {

void picture(auto to, point top_left, auto const& from) {
  auto size = from.get_size();
  for (auto y : std::views::iota(0, size.cy))
    for (auto x : std::views::iota(0, size.cx))
      to.write(top_left + architecture::size{x, y}, from.at(point{x, y}));
}
}  // namespace whole_picture::architecture::draw
