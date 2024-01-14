#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include <CrashHandlerInterop/CrashHandlerInterop.hpp>

// clang-format off
#include <Windows.h>
#include <DbgHelp.h>
// clang-format on

#include <ctime>
#include <fileapi.h>
#include <memory>
#include <psapi.h>
#include <shellapi.h>
#include <string>

void         ShowError(std::wstring const& msg) noexcept;
std::wstring FormatWin32ErrorMessage() noexcept;
std::wstring GetProcessName(HANDLE hnd) noexcept;
std::wstring CreateMiniDump(HANDLE procHnd, DWORD procId, MINIDUMP_EXCEPTION_INFORMATION ex, MINIDUMP_TYPE type) noexcept;
void         StartExternalProcess(std::wstring const& cli, std::wstring const& minidumpPath) noexcept;

namespace
{
struct RAIIHandle
{
    constexpr RAIIHandle(void* hnd) noexcept
        : hnd(hnd) {}
    ~RAIIHandle()
    {
        CloseHandle(hnd);
    }

    constexpr operator void*() noexcept
    {
        return hnd;
    }

    void* hnd;
};
struct RAIIArgv
{
    constexpr RAIIArgv(wchar_t** argv) noexcept
        : argv(argv) {}
    ~RAIIArgv()
    {
        LocalFree(argv);
    }

    constexpr operator void*() noexcept
    {
        return argv;
    }
    constexpr wchar_t* operator[](int n) noexcept
    {
        return argv[n];
    }

    wchar_t** argv;
};
} // namespace

constexpr int MONITORED_PROCESS_ID = 0;
constexpr int EVENT_NAME           = 1;
constexpr int MMIO_FILE_PATH       = 2;
constexpr int EXT_PROCESS_PATH     = 3;

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int)
{
    int      argc;
    RAIIArgv argv = CommandLineToArgvW(pCmdLine, &argc);

    if(argc < 3)
    {
        ShowError(L"Invalid command line arguments.");
        return -1;
    }

    DWORD procId = [wstr = argv[MONITORED_PROCESS_ID]]
    {
        std::string cs(wcslen(wstr), '\0');
        wcstombs(cs.data(), wstr, cs.size());
        return (DWORD)strtoul(cs.data(), nullptr, 0);
    }();

    RAIIHandle hndToMonitor = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | THREAD_ALL_ACCESS, FALSE, procId);
    if(hndToMonitor.hnd == NULL)
    {
        ShowError(FormatWin32ErrorMessage());
        return -2;
    }

    RAIIHandle crashEventHnd = OpenEventW(SYNCHRONIZE, FALSE, argv[EVENT_NAME]);
    if(crashEventHnd.hnd == NULL)
    {
        ShowError(FormatWin32ErrorMessage());
        return -3;
    }

    // Wait for the crash event to happen, or close if the monitored process isn't running anymore
    while(WaitForSingleObject(crashEventHnd, 5000) == WAIT_TIMEOUT)
    {
        DWORD exitCode = 0;
        if(GetExitCodeProcess(hndToMonitor, &exitCode) && exitCode != STILL_ACTIVE)
            return 0;
    }

    MINIDUMP_EXCEPTION_INFORMATION ex;
    CrashHandlerDumpType           type;
    if(!CrashHandlerReadData(argv[MMIO_FILE_PATH], &ex, &type))
    {
        ShowError(FormatWin32ErrorMessage());
        return -4;
    }

    constexpr MINIDUMP_TYPE smallDump = MINIDUMP_TYPE(MiniDumpWithCodeSegs |
                                                      MiniDumpWithDataSegs |
                                                      MiniDumpWithProcessThreadData |
                                                      MiniDumpWithoutOptionalData |
                                                      MiniDumpWithThreadInfo |
                                                      MiniDumpWithoutAuxiliaryState |
                                                      MiniDumpIgnoreInaccessibleMemory);

    constexpr MINIDUMP_TYPE fullDump = MINIDUMP_TYPE(MiniDumpWithFullMemory |
                                                     MiniDumpIgnoreInaccessibleMemory);

    auto const dumpPath = CreateMiniDump(hndToMonitor, procId, ex, type == FULL_DUMP ? fullDump : smallDump);
    TerminateProcess(hndToMonitor, -1);
    if(argc > 3)
        StartExternalProcess(argv[EXT_PROCESS_PATH], dumpPath);

    return 0;
}

void ShowError(std::wstring const& msg) noexcept
{
#ifndef NDEBUG
    MessageBoxW(NULL, msg.data(), L"Error", MB_ICONERROR | MB_OK);
#endif
}
std::wstring FormatWin32ErrorMessage() noexcept
{
    std::wstring err(4096, L'\0');
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err.data(), (DWORD)err.size(), NULL);
    return err;
}
std::wstring GetProcessName(HANDLE hnd) noexcept
{
    std::wstring name(256, L'\0');
    DWORD        sz = GetProcessImageFileNameW(hnd, name.data(), (DWORD)name.size());
    if(sz == 0)
        ShowError(L"Couldn't retrieve process name\n" + FormatWin32ErrorMessage()), std::exit(-1);

    name.resize(sz);
    auto sep = name.find_last_of(L'\\');
    auto est = name.find_last_of(L'.');
    return name.substr(sep != name.npos ? sep + 1 : 0, est != name.npos ? est - sep : name.npos);
}
std::wstring CreateMiniDump(HANDLE procHnd, DWORD procId, MINIDUMP_EXCEPTION_INFORMATION ex, MINIDUMP_TYPE type) noexcept
{
    std::wstring minidumpname;

    {
        std::time_t const t   = std::time(nullptr);
        std::tm const     now = *std::localtime(&t);

        // yyyyMMddHHmmss.dmp
        char      fmt[20] = {};
        int const sz      = snprintf(fmt, 20, "%04d%02d%02d%02d%02d%02d.dmp",
                                     now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);

        minidumpname.resize(sz);
        mbstowcs(minidumpname.data(), fmt, minidumpname.size());
    }

    RAIIHandle dmphnd = CreateFile(minidumpname.data(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(dmphnd == INVALID_HANDLE_VALUE || !MiniDumpWriteDump(procHnd, procId, dmphnd, type, ex.ExceptionPointers ? &ex : nullptr, NULL, NULL))
    {
        ShowError(L"Couldn't create minidump file\n" + minidumpname + L"\n" + FormatWin32ErrorMessage());
        return {};
    }
    return minidumpname;
}
void StartExternalProcess(std::wstring const& processExePath, std::wstring const& minidumpPath) noexcept
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // "MyProgram.exe" "minidump_name"
    // Adds quotes to the executable path to prevent malicous program execution due to whitespaces.
    std::wstring cli = L"\"" + processExePath + L"\"" + L" " + minidumpPath + L"";

    std::wstring workDir(256, L'\0');
    workDir.resize(GetCurrentDirectory(workDir.size(), workDir.data()));

    if(CreateProcess(NULL, cli.data(), NULL, NULL, FALSE,
                     CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS | CREATE_UNICODE_ENVIRONMENT,
                     NULL, workDir.data(), &si, &pi))
    {
        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}
