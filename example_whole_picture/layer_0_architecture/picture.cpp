#include <example_whole_picture/layer_0_architecture/picture.hpp>
#include <example_whole_picture/layer_0_architecture/draw/picture.hpp>
#include <iostream>

using namespace whole_picture;
using namespace whole_picture::architecture;

VV_RUNTIME_STATIC(picture)
runtime::class_<picture>::implements<surface> __;

picture::picture(std::size_t size_x, std::size_t size_y)
    : lines_(size_y, make_line(size_x)) {}

size picture::get_size() const {
  return size{.cx = (int)lines_.at(0).size(), .cy = (int)lines_.size()};
}
void picture::write(point p, char ch) { at(p) = ch; }
void picture::flush() const {
  for (const auto& line : lines_) {
    for (const auto& ch : line) std::cout << ch;
    std::cout << "\n";
  }
}
