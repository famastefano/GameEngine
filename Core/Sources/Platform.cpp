#include "Core/Platform.hpp"

#if !defined(_WIN32) || !defined(_WIN64)
#    error Only Windows is supported
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace Core
{
std::wstring getLastPlatformError() noexcept
{
    std::wstring err(4096, L'\0');
    err.resize(FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        err.data(),
        (DWORD)err.size(),
        NULL));
    return err;
}
}