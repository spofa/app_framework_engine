#include "nplugins/np_framework.h"
#pragma comment(lib,"nplugins.lib")

using namespace std;
using namespace NP;

int main() {
	NP_Framework::Auto()->Load("OpenGLPlugin");
	NP_Framework::Auto()->Load("ZipPlugin");
	NP_Framework::PluginMapIter iter;
	for (iter = NP_Framework::Auto()->Begin(); iter != NP_Framework::Auto()->End(); iter++){
		if (std::get<0>(NP_Framework::Auto()->Tuple(iter))){
			std::get<0>(NP_Framework::Auto()->Tuple(iter))->ShowMessage("OK");
		}
		if (std::get<0>(NP_Framework::Auto()->Tuple(iter))){
			std::get<0>(NP_Framework::Auto()->Tuple(iter))->SendMessage("OK");
		}
	}
	NP_Framework::Auto()->Free("ZipPlugin");
	NP_Framework::Auto()->Release();
	return 0;
}
