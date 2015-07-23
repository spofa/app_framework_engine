#ifndef NPLUGINS_NP_OS_PLATFORM_H_
#define NPLUGINS_NP_OS_PLATFORM_H_
//////////////////////////////////////////////////////////////////////////
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define NPLUGINS_WINRT 1
#elif defined(WIN32) || defined(_WIN32)
#define OS_WIN 1
#else
#define OS_LINUX 1
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#if defined(NP_STATICLIB)
#define NPAPI
#else
#if defined(NP_EXPORT)
#define NPAPI __declspec(dllexport)
#else
#define NPAPI __declspec(dllimport)
#endif
#endif
#elif defined(__GNUC__)
#if defined(NP_STATICLIB)
#define NPAPI
#else
#if defined(NP_EXPORT)
#define NPAPI __attribute__ ((visibility ("default")))
#else
// If you use -fvisibility=hidden in GCC, exception handling and RTTI
// would break if visibility wasn't set during export _and_ import
// because GCC would immediately forget all type infos encountered.
// See http://gcc.gnu.org/wiki/Visibility
#define NPAPI __attribute__ ((visibility ("default")))
#endif
#endif
#else
#error Unknown compiler, please implement shared library macros
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(OS_WIN)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(OS_LINUX)
#include <dlfcn.h>
#else
#error Please implement the shared library functions for your system
#endif
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <list>
#include <tuple>
#include <memory>
#include <mutex>
#include <iostream>
//////////////////////////////////////////////////////////////////////////
#endif
