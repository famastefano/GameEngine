#pragma once

#pragma once

#if GE_BUILD_ENABLE_MONOLITHIC
#  define SHARED_API
#else
#  ifdef SHARED_API_EXPORTS
#    define SHARED_API __declspec(dllexport)
#  else
#    define SHARED_API __declspec(dllimport)
#  endif
#endif
