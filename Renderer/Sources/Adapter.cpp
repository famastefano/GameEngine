#include "Renderer/Adapter.hpp"

#include "Core/AssertionMacros.hpp"
#include "Renderer/Sources/LogRenderer.hpp"

#include <dxgi1_6.h>

namespace Renderer
{
Adapter::Adapter(void* nativeHandle, std::wstring description, Vendor vendor, uint64_t dedicatedVideoMemory, uint64_t dedicatedSystemMemory, uint64_t sharedSystemMemory) noexcept
    : nativeHandle(nativeHandle)
    , description(std::move(description))
    , vendor(vendor)
    , dedicatedVideoMemory(dedicatedVideoMemory)
    , dedicatedSystemMemory(dedicatedSystemMemory)
    , sharedSystemMemory(sharedSystemMemory)
{
    assertf(nativeHandle != nullptr, L"Invalid Adapter handle.");
}
Adapter::Adapter(Adapter const& other) noexcept
    : Adapter(other.nativeHandle,
              other.description,
              other.vendor,
              other.dedicatedVideoMemory,
              other.dedicatedSystemMemory,
              other.sharedSystemMemory)
{
}
Adapter::~Adapter()
{
    IDXGIAdapter4* p = static_cast<IDXGIAdapter4*>(nativeHandle);
    p->Release();
}
void* Adapter::handle() noexcept
{
    return nativeHandle;
}
}