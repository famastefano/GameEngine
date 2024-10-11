#pragma once

#undef assert

#define check(cnd, ...) if(!(cnd)) __debugbreak()

#define assert(...) check(__VA_ARGS__)
