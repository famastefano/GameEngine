#include "ExceptionHandlers.hpp"

#include "CrashHandlerInterop/CrashHandlerInterop.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <atomic>
#include <exception>
#include <tlhelp32.h>

namespace
{
constexpr auto crashHandlerExe     = L"CrashHandler.exe";
constexpr auto systemWideEventName = L"FS_GAME_ENGINE_CRASH_EVENT";
constexpr auto crashInfoPath       = L"FS_GAME_ENGINE_CRASH_EVENT_CRASH_INFO";
HANDLE         crashEventHandle    = NULL;
}

static LONG WINAPI signalDebugger(EXCEPTION_POINTERS*);
static LONG WINAPI dumpAndCrash(EXCEPTION_POINTERS* p);
static LONG WINAPI flushAndCrash(EXCEPTION_POINTERS*);
static void        suspendAllThreadsExceptCurrent();

void registerExceptionHandler()
{
    if(IsDebuggerPresent())
    {
        SetUnhandledExceptionFilter(signalDebugger);
        std::set_terminate(+[]
                           { signalDebugger(nullptr); });
        return;
    }

    bool r = CrashHandlerStartHandler(
        GetCurrentProcessId(),
        crashHandlerExe,
        systemWideEventName,
        crashInfoPath,
        nullptr,
        &crashEventHandle);

    if(r)
    {
        SetUnhandledExceptionFilter(dumpAndCrash);
        std::set_terminate(+[]
                           { dumpAndCrash(nullptr); });
    }
    else
    {
        SetUnhandledExceptionFilter(flushAndCrash);
        std::set_terminate(+[]
                           { flushAndCrash(nullptr); });
    }
}

static LONG WINAPI signalDebugger(EXCEPTION_POINTERS*)
{
    __debugbreak();
    std::abort();
}

static LONG WINAPI dumpAndCrash(EXCEPTION_POINTERS* p)
{
    // In case someone attached it later
    if(IsDebuggerPresent())
        __debugbreak();

    // Allows only 1 thread to be the crashing thread
    static std::atomic_bool crashing = false;
    if(!crashing)
        crashing = true;
    else
        while(crashing.load(std::memory_order_relaxed))
            ;

    // TODO: flush all loggers

    CrashHandlerWriteData(crashInfoPath, GetCurrentThreadId(), p, CrashHandlerDumpType::FULL_DUMP);
    SetEvent(crashEventHandle);

    MessageBoxW(NULL, L"The application cannot continue due to a fatal error.", L"Fatal Error", MB_ICONERROR | MB_OK);

    // Wait for the crash handler to kill us
    while(crashing.load(std::memory_order_relaxed))
        Sleep(INFINITE);

    return 0;
}

static LONG WINAPI flushAndCrash(EXCEPTION_POINTERS*)
{
    // In case someone attached it later
    if(IsDebuggerPresent())
        __debugbreak();

    // TODO: flush all loggers

    return EXCEPTION_EXECUTE_HANDLER;
}

static void suspendAllThreadsExceptCurrent()
{
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;

    THREADENTRY32 te32{};
    CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    te32.dwSize = sizeof(THREADENTRY32);

    if(!Thread32First(hThreadSnap, &te32))
    {
        CloseHandle(hThreadSnap);
        return;
    }

    DWORD const thisThread  = GetCurrentThreadId();
    DWORD const thisProcess = GetCurrentProcessId();

    HANDLE threadHandle = INVALID_HANDLE_VALUE;
    do
    {
        if(te32.th32OwnerProcessID == thisProcess && te32.th32ThreadID != thisThread)
        {
            threadHandle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
            if(threadHandle != NULL)
            {
                SuspendThread(threadHandle);
                CloseHandle(threadHandle);
            }
        }
    } while(Thread32Next(hThreadSnap, &te32));
    CloseHandle(hThreadSnap);
}