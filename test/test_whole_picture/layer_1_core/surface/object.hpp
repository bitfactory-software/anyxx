#pragma once

#include <assert.h>

#include <bit_factory/anyxx.hpp>
#include <initializer_list>
#include <string_view>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif  //  _MSC_VER

namespace whole_picture::core {

class CORE_EXPORT surface {
 public:
  using line = std::vector<char>;

 private:
  std::vector<line> lines_;
  static auto make_line(std::size_t size_x) { return line(size_x, ' '); }

 public:
  surface(architecture::size size);
  surface(std::initializer_list<std::string_view> const& lines);
  architecture::size get_size() const;
  void write(whole_picture::architecture::point p, char ch);
  void draw(
      whole_picture::architecture::mutable_observed_surface const& surface,
      whole_picture::architecture::point p, char ch);
  bool contains(whole_picture::architecture::point p) const;
  auto& at(this auto& self, whole_picture::architecture::point p) {
    assert(self.contains(p));
    return self.lines_[p.y][p.x];
  }

  void flush() const;
};

}  // namespace whole_picture::core

//ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::surface)
//ANY_MODEL_FWD(CORE_EXPORT, whole_picture::core::surface,
//              whole_picture::architecture, surface)