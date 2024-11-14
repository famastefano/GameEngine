#include <Core/Platform/Platform.h>
#include <Windows.h>

namespace Core
{
bool IsDebuggerAttached()
{
  return IsDebuggerPresent();
}
} // namespace Core
