#pragma once

#include <Core/API.h>

namespace Core
{
template<typename CharT>
class String;

CORE_API bool IsDebuggerAttached();
CORE_API void DebugBreak();

CORE_API void GetLastErrorString(String<char>& Err);
} // namespace Core
