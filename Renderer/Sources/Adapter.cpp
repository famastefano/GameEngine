#include "Renderer/Adapter.hpp"

#include "Core/AssertionMacros.hpp"
#include "Renderer/Sources/LogRenderer.hpp"

#include <dxgi1_6.h>

namespace Renderer
{
Adapter::Adapter(Private::AdapterInterface* adapter, std::wstring description, Vendor vendor, uint64_t dedicatedVideoMemory, uint64_t dedicatedSystemMemory, uint64_t sharedSystemMemory) noexcept
    : adapter(adapter)
    , description(std::move(description))
    , vendor(vendor)
    , dedicatedVideoMemory(dedicatedVideoMemory)
    , dedicatedSystemMemory(dedicatedSystemMemory)
    , sharedSystemMemory(sharedSystemMemory)
{
    assertf(adapter != nullptr, L"Invalid Adapter handle.");
}
Adapter::Adapter(Adapter const& other) noexcept
    : Adapter(other.adapter,
              other.description,
              other.vendor,
              other.dedicatedVideoMemory,
              other.dedicatedSystemMemory,
              other.sharedSystemMemory)
{
    if(adapter)
        adapter->AddRef();
}
Adapter::~Adapter()
{
    if(adapter)
        adapter->Release();
}
Private::AdapterInterface* Adapter::handle() noexcept
{
    return adapter;
}
}