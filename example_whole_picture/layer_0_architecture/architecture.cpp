#include "architecture.hpp"

using namespace virtual_void;
using namespace whole_picture::architecture;

std::string whole_picture::architecture::hello() { return "hello"; }

namespace {

struct terminal {
  void write(point, char){}
  void flush() const {}
};

}  // namespace

VV_RUNTIME_STATIC(terminal)
runtime::class_<terminal>::implements<surface> __;

unique_surface whole_picture::architecture::screen() {
  return std::make_unique<terminal>();
}
