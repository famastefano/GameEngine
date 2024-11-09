#pragma once

#include <Core/Definitions.h>

namespace Core
{
template <typename T>
concept Sortable = requires(T a, T b) { a < b ? 0 : 1; };

template <typename T>
concept Hashable = requires(T a) { a.CalculateHash(); };
}
