#include "Renderer/Output.hpp"

#include "Core/AssertionMacros.hpp"
#include "Renderer/Sources/LogRenderer.hpp"

#include <dxgi1_6.h>

namespace Renderer
{
Output::Output(void* nativeHandle, std::wstring description) noexcept
    : nativeHandle(nativeHandle)
    , description(std::move(description))
{
    assertf(nativeHandle != nullptr, L"Invalid Output handle.");
}

Output::Output(Output const& other) noexcept
    : Output(other.nativeHandle, other.description)
{
}

Output::~Output()
{
    IDXGIOutput6* p = static_cast<IDXGIOutput6*>(nativeHandle);
    p->Release();
}

void* Output::handle() noexcept
{
    return nativeHandle;
}
}
