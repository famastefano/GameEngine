#pragma once

#include "Renderer/RHI.hpp"
#include "Renderer/Vendor.hpp"

#include <string>

namespace Renderer
{
class Adapter
{
    Private::AdapterInterface* adapter;

    public:
    Adapter(Private::AdapterInterface* adapter,
            std::wstring               description,
            Vendor                     vendor,
            uint64_t                   dedicatedVideoMemory,
            uint64_t                   dedicatedSystemMemory,
            uint64_t                   sharedSystemMemory) noexcept;
    Adapter(Adapter const& other) noexcept;
    ~Adapter();

    Private::AdapterInterface* handle() noexcept;

    std::wstring const description;
    Vendor const       vendor;
    uint64_t const     dedicatedVideoMemory;
    uint64_t const     dedicatedSystemMemory;
    uint64_t const     sharedSystemMemory;
};
}