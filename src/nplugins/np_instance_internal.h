#ifndef NPLUGINS_NP_INSTANCE_INTERNAL_H_
#define NPLUGINS_NP_INSTANCE_INTERNAL_H_

#include "nplugins/np_os_platform.h"
#include "nplugins/np_exports_address.h"
#include <string>

namespace NP {
	class NP_InstanceInterface;
	class NP_Framework;
	class NP_InstanceInternal {
	public: 
		NP_InstanceInternal();
		explicit NP_InstanceInternal(const std::string &filename);
		explicit NP_InstanceInternal(const NP_InstanceInternal &other);
		~NP_InstanceInternal();
		void Load(const std::string &plugins_name);
		void NP_Shutdown(NP_InstanceInterface &plugins_interface);
		void NP_Initialize(NP_InstanceInterface &plugins_interface);
		NP_ExportsAddress::HandleType Handle() const;
	private: 
		NP_InstanceInternal &operator =(const NP_InstanceInternal &other) = delete;
		NP_ExportsAddress::HandleType sharedLibraryHandle;
		size_t *referenceCount;
		using NP_SHUTDOWN_FUNCTION = void(*)(NP_InstanceInterface&);
		using NP_INITIALIZE_FUNCTION = void(*)(NP_InstanceInterface&);
		NP_SHUTDOWN_FUNCTION NP_ShutdownAPI;
		NP_INITIALIZE_FUNCTION NP_InitializeAPI;
	};
}
#endif
