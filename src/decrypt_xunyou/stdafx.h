// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "decrypt_xunyou/targetver.h"
#if defined(OS_WIN)
#include <SDKDDKVer.h>
#include <WinSock2.h>
#include <wincrypt.h>
#include <Shlwapi.h>
#include <UrlMon.h>
#include <atlconv.h>
#include <winhttp.h>
#pragma comment(lib,"urlmon.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"winhttp.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")
#pragma comment(lib,"zlibstat.lib")
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Cryptnet.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"libevent.lib")
#pragma comment(lib,"libevent_core.lib")
#pragma comment(lib,"libevent_extras.lib")
#pragma comment(lib,"base.lib")
#pragma comment(lib,"googleurl.lib")
#pragma comment(lib,"icuuc.lib")
#pragma comment(linker,"/NODEFAULTLIB:library")
#include <cassert>
#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <fstream>
#endif



// TODO: reference additional headers your program requires here
