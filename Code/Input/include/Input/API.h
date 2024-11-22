#pragma once

#if GE_BUILD_ENABLE_MONOLITHIC
#  define INPUT_API
#else
#  ifdef INPUT_API_EXPORTS
#    define INPUT_API __declspec(dllexport)
#  else
#    define INPUT_API __declspec(dllimport)
#  endif
#endif
