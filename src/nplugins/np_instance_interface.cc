#include "nplugins/np_instance_interface.h"

namespace NP{
	NP_InstanceInterface::NP_InstanceInterface(){
		plugins_name = "";
		send_message = nullptr;
		gdriver = nullptr;
	}
}