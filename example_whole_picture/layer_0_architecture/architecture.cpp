#include "architecture.hpp"

#include <array>
#include <ranges>

using namespace virtual_void;
using namespace whole_picture::architecture;

std::string whole_picture::architecture::hello() { return "hello"; }

namespace {

struct terminal {
  std::vector<std::vector<char> > buffer_;
  terminal(std::size_t size_x, std::size_t size_y) {
    for (std::size_t line : std::views::iota(0, (int)size_y))
      buffer_.emplace_back(std::vector<char>(size_x, ' ' ));
  }
  void write(point, char) {}
  void flush() const {}
};

}  // namespace

VV_RUNTIME_STATIC(terminal)
runtime::class_<terminal>::implements<surface> __;

unique_surface whole_picture::architecture::screen(std::size_t size_x,
                                                   std::size_t size_y) {
  return std::make_unique<terminal>(size_x, size_y);
}
