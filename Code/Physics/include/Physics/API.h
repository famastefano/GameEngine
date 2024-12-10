#pragma once

#if GE_BUILD_ENABLE_MONOLITHIC
#  define PHYSICS_API
#else
#  ifdef PHYSICS_API_EXPORTS
#    define PHYSICS_API __declspec(dllexport)
#  else
#    define PHYSICS_API __declspec(dllimport)
#  endif
#endif
