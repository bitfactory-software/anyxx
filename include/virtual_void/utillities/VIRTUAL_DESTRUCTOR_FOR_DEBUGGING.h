#pragma once

#ifdef _DEBUG
#define VIRTUAL_DESTRUCTOR_FOR_DEBUGGING virtual
#else
#define VIRTUAL_DESTRUCTOR_FOR_DEBUGGING
#endif  // DEBUG
