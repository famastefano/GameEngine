#include "Renderer/Output.hpp"

#include "Core/AssertionMacros.hpp"
#include "Renderer/Sources/LogRenderer.hpp"

#include <dxgi1_6.h>

namespace Renderer
{
Output::Output(Private::OutputInterface* output, std::wstring description) noexcept
    : output(output)
    , description(std::move(description))
{
    assertf(output != nullptr, L"Invalid Output.");
}

Output::Output(Output const& other) noexcept
    : Output(other.output, other.description)
{
    if(output)
        output->AddRef();
}

Output::~Output()
{
    if(output)
        output->Release();
}

Private::OutputInterface* Output::handle() noexcept
{
    return output;
}
}
