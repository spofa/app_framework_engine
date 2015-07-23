#ifndef NPLUGINS_NP_INSTANCE_INTERFACE_H_
#define NPLUGINS_NP_INSTANCE_INTERFACE_H_

#include "nplugins/np_os_platform.h"
#include "nplugins/np_instance_internal.h"

namespace NP{
	//////////////////////////////////////////////////////////////////////////
	class Renderer {};
	class Archive {};
	//////////////////////////////////////////////////////////////////////////
	class NP_InstanceInterface{
	public:
		class NP_SendMessage {
		public:
			virtual bool ShowMessage(const std::string& msg) = 0;
			virtual bool SendMessage(const std::string& msg) = 0;
		};
		class NP_GraphicsDriver {
		public:
			virtual const std::string &getName() const = 0;
			virtual std::auto_ptr<Renderer> createRenderer() = 0;
		};
		NP_InstanceInterface();
		std::string plugins_name;
		NP_InstanceInternal plugins_internal;
		NP_SendMessage* send_message;
		NP_GraphicsDriver* gdriver;
	};
}

#endif