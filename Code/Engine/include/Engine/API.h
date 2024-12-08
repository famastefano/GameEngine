#pragma once

#if GE_BUILD_ENABLE_MONOLITHIC
#  define ENGINE_API
#else
#  ifdef ENGINE_API_EXPORTS
#    define ENGINE_API __declspec(dllexport)
#  else
#    define ENGINE_API __declspec(dllimport)
#  endif
#endif
