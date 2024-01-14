#include "CrashHandlerInterop.hpp"

#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio> // swprintf_s

// clang-format off
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h> // event and process creation
#include <DbgHelp.h> // minidump
// clang-format on

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
} // namespace

constexpr auto file_size = sizeof(_MINIDUMP_EXCEPTION_INFORMATION::ThreadId) +
                           sizeof(_MINIDUMP_EXCEPTION_INFORMATION::ExceptionPointers) +
                           sizeof(CrashHandlerDumpType);

constexpr auto threadIdOffset          = 0;
constexpr auto exceptionPointersOffset = threadIdOffset + sizeof(_MINIDUMP_EXCEPTION_INFORMATION::ThreadId);
constexpr auto dumpTypeOffset          = exceptionPointersOffset + sizeof(_MINIDUMP_EXCEPTION_INFORMATION::ExceptionPointers);

bool CrashHandlerReadData(wchar_t const* filePath, void* exInfo, CrashHandlerDumpType* type)
{
    // https://stackoverflow.com/questions/13590980/how-do-i-get-at-the-exception-information-when-using-minidumpwritedump-out-of-pr

    // We read from a memory mapped file:
    // 1. the Thread ID of the thread that raised the exception
    // 2. the Exception Pointer
    // 3. the dump type (FULL|SMALL)

    RAIIHandle mapHnd = OpenFileMapping(FILE_MAP_READ, FALSE, filePath);
    if(mapHnd)
    {
        auto* info = static_cast<MINIDUMP_EXCEPTION_INFORMATION*>(exInfo);
        auto* data = static_cast<unsigned char*>(MapViewOfFile(mapHnd, FILE_MAP_READ, 0, 0, file_size));
        if(data)
        {
            memcpy(&info->ThreadId, data + threadIdOffset, sizeof(info->ThreadId));
            memcpy(&info->ExceptionPointers, data + exceptionPointersOffset, sizeof(info->ExceptionPointers));
            memcpy(type, data + dumpTypeOffset, sizeof(*type));
            info->ClientPointers = TRUE;
            return true;
        }
    }

    return false;
}

bool CrashHandlerWriteData(wchar_t const* filePath, unsigned long long threadId, void* exceptionPointer, CrashHandlerDumpType type)
{
    // We don't use a RAIIHandle because if we close it, the file would be deleted
    HANDLE mapHnd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, file_size, filePath);

    if(mapHnd)
    {
        unsigned char* data = static_cast<unsigned char*>(MapViewOfFile(mapHnd, FILE_MAP_WRITE, 0, 0, file_size));
        if(data)
        {
            decltype(_MINIDUMP_EXCEPTION_INFORMATION::ThreadId) thId = threadId;
            memcpy(data + threadIdOffset, &thId, sizeof(thId));
            memcpy(data + exceptionPointersOffset, &exceptionPointer, sizeof(exceptionPointer));
            memcpy(data + dumpTypeOffset, &type, sizeof(type));

            if(FlushViewOfFile(data, file_size) && UnmapViewOfFile(data))
                return true;
        }
    }
    return false;
}

bool CrashHandlerStartHandler(unsigned long long monitoredProcessId, wchar_t const* crashReporterExePath, wchar_t const* crashEventName, wchar_t const* crashExceptionInfoPath, wchar_t const* optionalExternalProcessPath, void** event_handle)
{
    HANDLE ev = CreateEvent(NULL, TRUE, FALSE, crashEventName);
    if(!ev)
        return false;

    // Adds quotes to the executable path to prevent malicious program execution due to whitespaces

    wchar_t cli[4096] = {};

    if(optionalExternalProcessPath)
    {
        swprintf_s(cli, LR"("%s" %llu "%s" "%s" "%s")",
                   crashReporterExePath,
                   monitoredProcessId,
                   crashEventName,
                   crashExceptionInfoPath,
                   optionalExternalProcessPath);
    }
    else
    {
        swprintf_s(cli, LR"("%s" %llu "%s" "%s")",
                   crashReporterExePath,
                   monitoredProcessId,
                   crashEventName,
                   crashExceptionInfoPath);
    }

    STARTUPINFOW        si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if(CreateProcess(nullptr, cli, nullptr, nullptr, FALSE, DETACHED_PROCESS | IDLE_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP | CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &si, &pi) != 0)
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        *event_handle = ev;
        return true;
    }

    SetEvent(ev);
    CloseHandle(ev);
    return false;
}