#pragma once

#if GE_BUILD_ENABLE_MONOLITHIC
#  define CORE_API
#else
#  ifdef CORE_API_EXPORTS
#    define CORE_API __declspec(dllexport)
#  else
#    define CORE_API __declspec(dllimport)
#  endif
#endif
