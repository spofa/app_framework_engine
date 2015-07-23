#include "nplugins/np_framework.h"

namespace NP {
	NP_Framework* NP_Framework::instance_ = nullptr;
	std::mutex NP_Framework::instance_lock_;
	NP_Framework::NP_Framework(){
		plugins.clear();
	}
	NP_Framework::NP_Framework(const NP_Framework &other){
		plugins = other.plugins;
	}
	NP_Framework::~NP_Framework(){
		Free();
	}
	NP_Framework* NP_Framework::Auto(){
		instance_lock_.lock();
		if(instance_==nullptr){
			instance_ = new NP_Framework;
		}
		instance_lock_.unlock();
		return instance_;
	}
	void NP_Framework::Release(){
		instance_lock_.lock();
		if(instance_!=nullptr){
			delete instance_;
			instance_ = nullptr;
		}
		instance_lock_.unlock();
	}
	void NP_Framework::Load(const std::string& plugins_name) {
		if (this->plugins.find(plugins_name) == this->plugins.end()) {
			NP_InstanceInterface class_interface;
			class_interface.plugins_name = plugins_name;
			class_interface.plugins_internal.Load(plugins_name);
			if (!class_interface.plugins_internal.Handle()){
				return;
			}
			class_interface.plugins_internal.NP_Initialize(class_interface);
			this->plugins.insert(PluginMap::value_type(plugins_name, class_interface));
		}
	}
	void NP_Framework::Free(const std::string& plugins_name){
		PluginMapIter iter;
		for (iter = Begin(); iter != End(); iter++){
			if(iter->first==plugins_name){
				NP_InstanceInterface class_interface(iter->second);
				class_interface.plugins_internal.NP_Shutdown(class_interface);
				plugins.erase(plugins.find(iter->first));
				break;
			}
		}
	}
	void NP_Framework::Free(){
		PluginMapIter iter;
		for (iter = Begin(); iter != End(); iter++){
			NP_InstanceInterface class_interface(iter->second);
			class_interface.plugins_internal.NP_Shutdown(class_interface);
		}
		plugins.clear();
	}
	NP_Framework::PluginMapIter NP_Framework::Begin(){
		return plugins.begin();
	}
	NP_Framework::PluginMapIter NP_Framework::End(){
		return plugins.end();
	}
	NP_Framework::PluginTuple NP_Framework::Tuple(NP_Framework::PluginMapIter& iter){
		return (std::make_tuple(iter->second.send_message, iter->second.gdriver));
	}
}