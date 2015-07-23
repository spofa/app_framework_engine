#ifndef SSL_OBSERVER_WIN_DOWNLOAD_H_
#define SSL_OBSERVER_WIN_DOWNLOAD_H_
//////////////////////////////////////////////////////////////////////////
#include "ssl_observer/main.h"
//////////////////////////////////////////////////////////////////////////
namespace ssl_observer{
	class WinDownload
	{
	public:
		WinDownload(void);
		~WinDownload(void);
		bool CreateConnect(const wchar_t* domain,const char* port =NULL);
		bool AddDownloadHeader(const wchar_t* path,const wchar_t* header = NULL);
		std::string Download(const size_t try_count=1,const char* file = NULL);
	private:
		WinDownload(const WinDownload&) = delete;
		WinDownload& operator=(const WinDownload&) = delete;
		void DownloadDone();
		HINTERNET net_open_;
		HINTERNET net_connect_;
		HINTERNET net_request_;
	};
}
//////////////////////////////////////////////////////////////////////////
#endif