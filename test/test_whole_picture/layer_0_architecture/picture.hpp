#pragma once

#include <assert.h>

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <initializer_list>
#include <vector>
#include <string_view>

namespace whole_picture::architecture {

class ARCHITECTURE_EXPORT picture {
 public:
  using line = std::vector<char>;

 private:
  std::vector<line> lines_;
  static auto make_line(std::size_t size_x) { return line(size_x, ' '); }

 public:
  picture(std::size_t size_x, std::size_t size_y);
  picture(std::initializer_list<std::string_view> const& lines);
  size get_size() const;
  void write(point p, char ch);
  void draw(mutable_observed_surface const& surface, point p, char ch);
  auto& at(this auto& self, point p) {
    assert(p.x >= 0 && p.x < static_cast<int>(self.lines_[0].size()));
    assert(p.y >= 0 && p.y < static_cast<int>(self.lines_.size()));
    return self.lines_[p.y][p.x];
  }

  void flush() const;
};

}  // namespace whole_picture::architecture

ANY_META_CLASS_FWD(ARCHITECTURE_EXPORT, whole_picture::architecture::picture)
ANY_MODEL_FWD(ARCHITECTURE_EXPORT, whole_picture::architecture::picture, whole_picture::architecture, surface)