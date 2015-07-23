#ifndef NPLUGINS_NP_EXPORTS_ADDRESS_H_
#define NPLUGINS_NP_EXPORTS_ADDRESS_H_

#include "nplugins/np_os_platform.h"

namespace NP {
#if (defined(OS_WIN)||defined(OS_LINUX))
	class NP_ExportsAddress {
	public:
#if defined(OS_WIN)
		typedef FARPROC Function;
		typedef HMODULE HandleType;
#else
		typedef void* Function;
		typedef void* HandleType;
#endif
		static HandleType Load(const std::string &path);
		static void Unload(HandleType sharedLibraryHandle);
		static Function GetAddress(HandleType sharedLibraryHandle, const std::string &functionName);
	};
#else
#error Please implement the shared library functions for your system
#endif
}

#endif
