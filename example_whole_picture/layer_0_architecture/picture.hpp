#pragma once

#include <assert.h>

#include <example_whole_picture/layer_0_architecture/architecture.hpp>
#include <vector>

namespace whole_picture::architecture {

class ARCHITECTURE_EXPORT picture {
  using line = std::vector<char>;
  std::vector<line> lines_;

  static auto make_line(std::size_t size_x) { return line(size_x, ' '); }

 public:
  picture(std::size_t size_x, std::size_t size_y);
  size get_size() const;
  void write(point p, char ch);
  void draw(mutable_observed_surface const& surface, point p, char ch);
  auto& at(this auto& self, point p) {
    assert(p.x >= 0 && p.x < self.lines_[0].size());
    assert(p.y >= 0 && p.y < self.lines_.size());
    return self.lines_[p.y][p.x];
  }

  void flush() const;
};

}  // namespace whole_picture::architecture

VV_RUNTIME(ARCHITECTURE_EXPORT, whole_picture::architecture::picture)
