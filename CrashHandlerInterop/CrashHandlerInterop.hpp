#pragma once

#ifdef __cplusplus
#    include <cstdint>
#else
#    include <stdint.h>
#endif

#ifndef _WIN32
#    error "This library requires Windows"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    enum CrashHandlerDumpType : uint8_t
    {
        /// <summary>
        /// Dumps all the available information.<para></para>
        /// Warning: Creates the largest dump possible.
        /// </summary>
        FULL_DUMP = 1,

        /// <summary>
        /// Creates a dump with Code, Data, Thread, and Process information.
        /// </summary>
        SMALL_DUMP = 2,
    };

    /// <summary>
    /// Reads the data written by crash_reporter_write_data.
    /// </summary>
    /// <param name="filePath">Unique file path.</param>
    /// <param name="exInfo">Exception info, if present.</param>
    /// <param name="type">Which dump is requested to be done.</param>
    /// <returns>If the data has been read successfully.</returns>
    bool __declspec(dllexport) CrashHandlerReadData(
        wchar_t const*        filePath,
        void*                 exInfo,
        CrashHandlerDumpType* type);

    /// <summary>
    /// Writes exception info to a file handled by the system.
    /// </summary>
    /// <param name="filePath">Unique file path.</param>
    /// <param name="threadId">ID of the thread that raised an exception.</param>
    /// <param name="exceptionPointer">If it's a structured exception, pass the exception pointer here.</param>
    /// <param name="type">Which dump we want to do.</param>
    /// <returns>If the data has been written successfully.</returns>
    bool __declspec(dllexport) CrashHandlerWriteData(
        wchar_t const*       filePath,
        unsigned long long   threadId,
        void*                exceptionPointer,
        CrashHandlerDumpType type);

    /// <summary>
    /// Spawns the crash handler with the given options.
    /// </summary>
    /// <param name="monitoredProcessId">ID of the process to monitor for crashes.</param>
    /// <param name="crashReporterExePath">Path to the crash reporter tool executable.</param>
    /// <param name="crashEventName">Name of the event used to signal that we are crashing. Shall be system-wide unique.</param>
    /// <param name="crashExceptionInfoPath">Path to the file used by crash_reporter_write_data</param>
    /// <param name="optionalExternalProcessPath">Optional.Path to another executable that will be started after the dump has been generated.<para></para>The dump's path, relative to the crash reporter tool executable, will be passed as the 1st argument.</param>
    /// <param name="event_handle">HANDLE to a Win32 event where we'll signal that we are crashing.</param>
    /// <returns>If the handler has been spawned successfully.</returns>
    bool __declspec(dllexport) CrashHandlerStartHandler(
        unsigned long long monitoredProcessId,
        wchar_t const*     crashReporterExePath,
        wchar_t const*     crashEventName,
        wchar_t const*     crashExceptionInfoPath,
        wchar_t const*     optionalExternalProcessPath,
        void**             event_handle);

#ifdef __cplusplus
}
#endif