#include <example_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <ranges>

using namespace whole_picture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

void line::draw(architecture::mutable_observed_surface const& surface) const {
  const auto ch = '*';
 // auto start = point{}
 // for( int x = std::min( p1.x, p2.x ); x < std::max( p1.x, p2.x ); ++x
	//for( int y = std::min( p1.y, p2.y ); x < std::max( p1.y, p2.y ); ++x

  surface.write(p1, ch);
  surface.write(p2, ch);
}
