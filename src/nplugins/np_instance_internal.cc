#define NP_EXPORT 1
#include "nplugins/np_instance_internal.h"
using namespace std;

namespace NP {
	NP_InstanceInternal::NP_InstanceInternal():
		sharedLibraryHandle(0),
		referenceCount(0){
		this->NP_ShutdownAPI = nullptr;
		this->NP_InitializeAPI = nullptr;
	}
	NP_InstanceInternal::NP_InstanceInternal(const std::string &filename):
		sharedLibraryHandle(0),
		referenceCount(0){
		Load(filename);
	}
	NP_InstanceInternal::NP_InstanceInternal(const NP_InstanceInternal &other) :
		sharedLibraryHandle(other.sharedLibraryHandle),
		referenceCount(other.referenceCount),
		NP_ShutdownAPI(other.NP_ShutdownAPI),
		NP_InitializeAPI(other.NP_InitializeAPI) {
		if (this->referenceCount) {
			++(*this->referenceCount);
		}
	}
	NP_InstanceInternal::~NP_InstanceInternal() {
		int remainingReferences = --*(this->referenceCount);
		if (remainingReferences == 0) {
			delete this->referenceCount;
			NP_ExportsAddress::Unload(this->sharedLibraryHandle);
		}
	}
	void NP_InstanceInternal::Load(const std::string &plugins_name){
		this->sharedLibraryHandle = NP_ExportsAddress::Load(plugins_name);
		try {
			NP_ExportsAddress::Function function = nullptr;
			function = NP_ExportsAddress::GetAddress(this->sharedLibraryHandle, "NP_Shutdown");
			this->NP_ShutdownAPI = reinterpret_cast<NP_SHUTDOWN_FUNCTION>(function);
			function = nullptr;
			function = NP_ExportsAddress::GetAddress(this->sharedLibraryHandle, "NP_Initialize");
			this->NP_InitializeAPI = reinterpret_cast<NP_INITIALIZE_FUNCTION>(function);
			this->referenceCount = new size_t(1);
		}
		catch (std::exception &) {
			NP_ExportsAddress::Unload(this->sharedLibraryHandle);
			throw;
		}
	}
	void NP_InstanceInternal::NP_Shutdown(NP_InstanceInterface &plugins_interface) {
		if (this->NP_ShutdownAPI!=nullptr){
			return this->NP_ShutdownAPI(plugins_interface);
		}
	}
	void NP_InstanceInternal::NP_Initialize(NP_InstanceInterface &plugins_interface) {
		if (this->NP_InitializeAPI!=nullptr){
			this->NP_InitializeAPI(plugins_interface);
		}
	}
	NP_ExportsAddress::HandleType NP_InstanceInternal::Handle() const{
		return sharedLibraryHandle;
	}
}
