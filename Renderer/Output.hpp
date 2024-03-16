#pragma once

#include "Renderer/RHI.hpp"

#include <string>

namespace Renderer
{
class Output
{
    Private::OutputInterface* output;

    public:
    Output(Private::OutputInterface* output, std::wstring description) noexcept;
    Output(Output const& other) noexcept;
    ~Output();

    Private::OutputInterface* handle() noexcept;

    std::wstring const description;
};
}