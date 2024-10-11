#include <new>
#include <Core/Allocator/GlobalAllocator.h>
#include <Core/Platform/Platform.h>

namespace Core
{
struct StdStaticInit
{
  StdStaticInit();
};
static StdStaticInit _stdStaticInit;
} // namespace Core

Core::StdStaticInit::StdStaticInit()
{
  std::set_new_handler(+[] {
    if (IsDebuggerAttached())
      __debugbreak();
    std::terminate();
  });
}
