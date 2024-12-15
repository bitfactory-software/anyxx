#pragma once

namespace virtual_void{
struct static_init{
	static_init(auto f){f();}
};
}
