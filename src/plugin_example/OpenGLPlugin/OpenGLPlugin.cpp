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

using namespace std;

namespace NP {

	class OpenGLGraphicsDriver : 
		public NP_InstanceInterface::NP_GraphicsDriver,
		public NP_InstanceInterface::NP_SendMessage{
  public:
	virtual const std::string &getName() const {
		static string sName("OpenGL graphics driver");
		return sName;
    }
	auto_ptr<Renderer> createRenderer() {
		return auto_ptr<Renderer>(new Renderer());
    }
	virtual bool ShowMessage(const std::string& msg){
		if(msg=="OK"){
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

  /// <summary>Retrieve the engine version we're going to expect</summary>
  extern "C" __declspec(dllexport) void NP_Shutdown(NP_InstanceInterface &plugins_interface) {
	  if (plugins_interface.gdriver){
		  delete plugins_interface.gdriver;
		  plugins_interface.gdriver = nullptr;
		  plugins_interface.send_message = nullptr;
	  }
  }

  /// <summary>Register the plugin to an engine kernel</summary>
  /// <param name="kernel">Kernel the plugin will register to</summary>
  extern "C" __declspec(dllexport) void NP_Initialize(NP_InstanceInterface &plugins_interface) {
	  plugins_interface.gdriver = new OpenGLGraphicsDriver();
	  plugins_interface.send_message = dynamic_cast<NP_InstanceInterface::NP_SendMessage*>(plugins_interface.gdriver);
  }

} // namespace MyEngine