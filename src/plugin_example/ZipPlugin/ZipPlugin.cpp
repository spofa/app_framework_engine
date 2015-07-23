///////////////////////////////////////////////////////////////////////////////
// Plugin architecture example                                               //
//                                                                           //
// This code serves as an example to the plugin architecture discussed in    //
// the article and can be freely used.                                       //
///////////////////////////////////////////////////////////////////////////////
#include "nplugins/np_os_platform.h"
#include "nplugins/np_framework.h"
#include "nplugins/np_instance_interface.h"
#pragma comment(lib,"nplugins.lib")

#include <stdexcept>

using namespace std;

namespace NP {
	class ZipArchiveReader : public NP_InstanceInterface::NP_SendMessage {
	public:
		virtual bool ShowMessage(const std::string& msg){
			if (msg == "OK"){
				return true;
			}
			return false;
		}
		virtual bool SendMessage(const std::string& msg){
			if (msg == "OK"){
				return true;
			}
			return false;
		}
	};
	extern "C" __declspec(dllexport) void NP_Shutdown(NP_InstanceInterface &plugins_interface) {
		if (plugins_interface.send_message){
			delete plugins_interface.send_message;
			plugins_interface.send_message = nullptr;
		}
		if (plugins_interface.gdriver){
			delete plugins_interface.gdriver;
			plugins_interface.gdriver = nullptr;
		}
  }

  /// <summary>Registers the plugin to an engine kernel</summary>
  /// <param name="kernel">Kernel the plugin will be registered to</param>
	extern "C" __declspec(dllexport) void NP_Initialize(NP_InstanceInterface &plugins_interface) {
		plugins_interface.send_message = new ZipArchiveReader();
  }

}