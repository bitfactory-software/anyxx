#pragma once
//
#ifdef VV_SHARED
#pragma message("build VV_SHARED")
#define VV_EXPORT  __declspec(dllexport)
#else
#define VV_EXPORT
#pragma message("build not VV_SHARED")
#endif // !VV_EXPORT
