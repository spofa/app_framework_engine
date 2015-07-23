#ifndef NPLUGINS_NP_FRAMEWORK_H_
#define NPLUGINS_NP_FRAMEWORK_H_

#include "nplugins/np_os_platform.h"
#include "nplugins/np_instance_interface.h"

namespace NP {
	class NP_Framework {
	public:
		using PluginMap =  std::map<std::string, NP_InstanceInterface>;
		using PluginMapIter = std::map<std::string, NP_InstanceInterface>::iterator;
		using PluginTuple = std::tuple<NP_InstanceInterface::NP_SendMessage*, NP_InstanceInterface::NP_GraphicsDriver*>;
		NPAPI NP_Framework();
		NPAPI explicit NP_Framework(const NP_Framework &other);
		NPAPI ~NP_Framework();
		NPAPI static NP_Framework* Auto();
		NPAPI void Release();
		NPAPI void Load(const std::string& plugins_name);
		NPAPI void Free(const std::string& plugins_name);
		NPAPI void Free();
		NPAPI PluginMapIter Begin();
		NPAPI PluginMapIter End();
		NPAPI PluginTuple Tuple(PluginMapIter& iter);
	private:
		NP_Framework &operator =(const NP_Framework &other) = delete;
		static NP_Framework* instance_;
		static std::mutex instance_lock_;
		PluginMap plugins;
	};
}

#endif