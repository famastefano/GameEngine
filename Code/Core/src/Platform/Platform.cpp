#include <Core/Container/String.h>
#include <Core/Platform/Platform.h>
#include <Windows.h>

namespace Core
{
bool IsDebuggerAttached()
{
  return IsDebuggerPresent();
}
void DebugBreak()
{
  __debugbreak();
}
void GetLastErrorString(String<char>& Err)
{
  LPTSTR      lpMsgBuf{};
  const DWORD dw = GetLastError();

  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsgBuf,
      0, NULL);

  if (lpMsgBuf)
    Err.Assign(StringView<char>{lpMsgBuf});
  LocalFree(lpMsgBuf);
}
} // namespace Core
