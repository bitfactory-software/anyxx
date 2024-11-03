#pragma once

#include <typeindex>

namespace virtual_void {

class error;

}  // namespace virtual_void

#ifdef _DEBUG
#define VIRTUAL_DESTRUCTOR_FOR_DEBUGGING virtual
#else
#define VIRTUAL_DESTRUCTOR_FOR_DEBUGGING
#endif  // DEBUG
