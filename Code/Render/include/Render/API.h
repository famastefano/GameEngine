#pragma once

#if GE_BUILD_ENABLE_MONOLITHIC
#  define RENDER_API
#else
#  ifdef RENDER_API_EXPORTS
#    define RENDER_API __declspec(dllexport)
#  else
#    define RENDER_API __declspec(dllimport)
#  endif
#endif
