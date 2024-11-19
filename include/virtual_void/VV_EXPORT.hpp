#pragma once
//

#ifndef VV_EXPORT
#define VV_EXPORT
#endif

#ifdef VV_SHARED
#ifdef _MSVC_LANG
#undef VV_EXPORT
#define VV_EXPORT __declspec(dllexport)
#endif

#endif  // !VV_EXPORT
