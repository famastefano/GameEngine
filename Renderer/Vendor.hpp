#pragma once

#include <cstdint>

namespace Renderer
{
enum class Vendor : int32_t
{
    Unknown,
    NVIDIA,
    AMD,
    Intel,
};
}