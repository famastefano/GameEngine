#pragma once

#include "Renderer/Vendor.hpp"

#include <string>

namespace Renderer
{
class Adapter
{
    void* nativeHandle;

    public:
    Adapter(void*        nativeHandle,
            std::wstring description,
            Vendor       vendor,
            uint64_t     dedicatedVideoMemory,
            uint64_t     dedicatedSystemMemory,
            uint64_t     sharedSystemMemory) noexcept;
    Adapter(Adapter const& other) noexcept;
    ~Adapter();

    void* handle() noexcept;

    std::wstring const description;
    Vendor const       vendor;
    uint64_t const     dedicatedVideoMemory;
    uint64_t const     dedicatedSystemMemory;
    uint64_t const     sharedSystemMemory;
};
}