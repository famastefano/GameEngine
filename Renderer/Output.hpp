#pragma once

#include <string>

namespace Renderer
{
class Output
{
    void* nativeHandle;

    public:
    Output(void* nativeHandle, std::wstring description) noexcept;
    Output(Output const& other) noexcept;
    ~Output();

    void* handle() noexcept;

    std::wstring const description;
};
}