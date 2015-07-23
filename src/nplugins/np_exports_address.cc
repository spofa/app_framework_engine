#define NP_EXPORT 1
#include "nplugins/np_exports_address.h"

namespace NP {
#if defined(OS_WIN)
	NP_ExportsAddress::HandleType NP_ExportsAddress::Load(const std::string &path) {
		std::string pathWithExtension = path + ".dll";
		HMODULE moduleHandle = ::LoadLibraryA(pathWithExtension.c_str());
		if (moduleHandle == NULL) {
			throw std::runtime_error("Could not load DLL");
		}
		return moduleHandle;
	}
	void NP_ExportsAddress::Unload(HandleType sharedLibraryHandle) {
		BOOL result = ::FreeLibrary(sharedLibraryHandle);
		if (result == FALSE) {
			throw std::runtime_error("Could not unload DLL");
		}
	}
	NP_ExportsAddress::Function NP_ExportsAddress::GetAddress(HandleType sharedLibraryHandle, const std::string &functionName) {
		NP_ExportsAddress::Function functionAddress = ::GetProcAddress(sharedLibraryHandle, functionName.c_str());
		if (functionAddress == NULL) {
			throw std::runtime_error("Could not find exported function");
		}
		return reinterpret_cast<NP_ExportsAddress::Function>(functionAddress);
	}
#elif defined(OS_LINUX)
	HandleType NP_ExportsAddress::Load(const std::string &path) {
		std::string pathWithExtension = std::string("./lib") + path + ".so";

		void *sharedObject = ::dlopen(pathWithExtension.c_str(), RTLD_NOW);
		if (sharedObject == NULL) {
			throw std::runtime_error(
				std::string("Could not load '") + pathWithExtension + "'"
				);
		}

		return sharedObject;
	}
	void NP_ExportsAddress::Unload(HandleType sharedLibraryHandle) {
		int result = ::dlclose(sharedLibraryHandle);
		if (result != 0) {
			throw std::runtime_error("Could not unload shared object");
		}
	}
	template<typename TSignature>
	TSignature *NP_ExportsAddress::GetFunctionPointer(
		HandleType sharedLibraryHandle,const std::string &functionName) {
		::dlerror(); // clear error value

		void *functionAddress = ::dlsym(
			sharedLibraryHandle, functionName.c_str()
			);

		const char *error = ::dlerror(); // check for error
		if (error != NULL) {
			throw std::runtime_error("Could not find exported function");
		}

		return reinterpret_cast<TSignature *>(functionAddress);
	}

#else
#error Please implement the shared library functions for your system
#endif
}