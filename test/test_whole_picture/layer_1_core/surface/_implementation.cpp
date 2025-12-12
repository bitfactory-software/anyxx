#include <algorithm>
#include <iostream>
#include <test/test_whole_picture/layer_0_architecture/draw/picture.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::architecture;

//ANY_META_CLASS(core::surface)
//ANY_MODEL(core::surface, whole_picture::architecture, surface);

core::surface::surface(architecture::size size)
    : lines_(static_cast<std::size_t>(size.cy), make_line(static_cast<std::size_t>(size.cx))) {}

core::surface::surface(std::initializer_list<std::string_view> const& lines) {
  for (auto l : lines) lines_.emplace_back(l.begin(), l.end());
  auto witdth = std::ranges::fold_left(lines, 0U,
                                       [](std::size_t width, auto const& line) {
                                         return std::max(width, line.size());
                                       });
  for (auto& line : lines_) line.resize(witdth);
}

size core::surface::get_size() const {
  return size{.cx = (int)lines_.at(0).size(), .cy = (int)lines_.size()};
}
bool core::surface::contains(whole_picture::architecture::point p) const {
  return (p.x >= 0 && p.x < static_cast<int>(lines_[0].size())) &&
         (p.y >= 0 && p.y < static_cast<int>(lines_.size()));
}
void core::surface::write(point p, char ch) {
  if (contains(p)) at(p) = ch;
}
void core::surface::flush() const {
  for (const auto& line : lines_) {
    for (const auto& ch : line) std::cout << ch;
    std::cout << "\n";
  }
}
