#pragma once

#define GE_BIT(Bit) 1ull << Bit

#define GE_IMPL_STRINGIFY_HELPER(x) #x
#define GE_STRINGIFY(Symbol)        GE_IMPL_STRINGIFY_HELPER(Symbol)

#define GE_IMPL_JOIN(Symbol1, Symbol2) Symbol1##Symbol2
#define GE_JOIN(Symbol1, Symbol2) GE_IMPL_JOIN(Symbol1, Symbol2)
