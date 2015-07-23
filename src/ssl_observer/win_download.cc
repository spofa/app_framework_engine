#include "ssl_observer/win_download.h"
#include <fstream>

namespace ssl_observer{
	void FreeConfig(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* config) {
		if (config->lpszAutoConfigUrl)
			GlobalFree(config->lpszAutoConfigUrl);
		if (config->lpszProxy)
			GlobalFree(config->lpszProxy);
		if (config->lpszProxyBypass)
			GlobalFree(config->lpszProxyBypass);
	}

	void FreeInfo(WINHTTP_PROXY_INFO* info) {
		if (info->lpszProxy)
			GlobalFree(info->lpszProxy);
		if (info->lpszProxyBypass)
			GlobalFree(info->lpszProxyBypass);
	}
	bool ConfigureSSL(HINTERNET internet){
		DWORD protocols = 0;
		protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_SSL2;
		protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_SSL3;
		protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1;
		protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1;
		protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
		BOOL rv = WinHttpSetOption(internet, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));
		return (rv == TRUE);
	}
	bool ApplyProxy(HINTERNET internet, const wchar_t* proxy_str, bool is_direct){
		WINHTTP_PROXY_INFO pi;
		if (is_direct){
			pi.dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
			pi.lpszProxy = WINHTTP_NO_PROXY_NAME;
			pi.lpszProxyBypass = WINHTTP_NO_PROXY_BYPASS;
		}
		else{
			pi.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			pi.lpszProxy = const_cast<LPWSTR>(proxy_str);
			pi.lpszProxyBypass = WINHTTP_NO_PROXY_BYPASS;
		}
		BOOL rv = WinHttpSetOption(internet, WINHTTP_OPTION_PROXY, &pi, sizeof(pi));
		return (rv == TRUE);
	}

	bool ConfigureProxy(HINTERNET internet){
		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ie_config = { 0 };
		if (!WinHttpGetIEProxyConfigForCurrentUser(&ie_config)){
			return false;
		}
		WINHTTP_AUTOPROXY_OPTIONS options = { 0 };
		options.fAutoLogonIfChallenged = TRUE;
		if (ie_config.fAutoDetect){
			options.lpszAutoConfigUrl = ie_config.lpszAutoConfigUrl;
			options.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
		}
		else{
			options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
			options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
		}
		std::wstring query_url(L"http://www.baidu.com");
		WINHTTP_PROXY_INFO info = { 0 };
		BOOL rv = WinHttpGetProxyForUrl(internet, query_url.c_str(), &options, &info);
		WINHTTP_PROXY_INFO pi;
		switch (info.dwAccessType){
		case WINHTTP_ACCESS_TYPE_NO_PROXY:
			ApplyProxy(internet, NULL, TRUE);
			break;
		case WINHTTP_ACCESS_TYPE_NAMED_PROXY:
			ApplyProxy(internet, ie_config.lpszProxy, FALSE);
			break;
		default:
			pi.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			pi.lpszProxy = ie_config.lpszProxy;
			pi.lpszProxyBypass = ie_config.lpszProxyBypass;
			rv = WinHttpSetOption(internet, WINHTTP_OPTION_PROXY, &pi, sizeof(pi));
			break;
		}
		FreeConfig(&ie_config);
		FreeInfo(&info);
		return (rv == TRUE);
	}
    WinDownload::WinDownload(void): net_open_(NULL), net_connect_(NULL), net_request_(NULL){
		DownloadDone();
    }
    WinDownload::~WinDownload(void){
		DownloadDone();
    }
	bool WinDownload::CreateConnect(const wchar_t* domain,const char* port){
		const wchar_t *user_agent = L"iTunes/11.0.5 (Windows; Microsoft Windows 7 x64 Ultimate Edition Service Pack 1 (Build 7601)) AppleWebKit/7600.1017.0.24";
		net_open_ = WinHttpOpen(user_agent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if(!net_open_){
			return false;
		} 
		if(!ConfigureSSL(net_open_)){
			return false;
		}
		net_connect_ = WinHttpConnect(net_open_, domain, port == NULL ? INTERNET_DEFAULT_HTTP_PORT : atoi(port), 0);
		if(!net_connect_){
			return false;
		}
		return true;
	}
	bool WinDownload::AddDownloadHeader(const wchar_t* path,const wchar_t* header){
		const wchar_t *types[50] = {{L"*/*"}, {0}};
		net_request_ = WinHttpOpenRequest(net_connect_, L"GET", path, NULL, WINHTTP_NO_REFERER, types, WINHTTP_FLAG_BYPASS_PROXY_CACHE);
		if(!net_request_){
			return false;
		}
		if(header&&header[0]){
			if(!WinHttpAddRequestHeaders(net_request_, header, wcslen(header), WINHTTP_ADDREQ_FLAG_ADD)){
				return false;
			}
		}
		std::wstring some_header(L"Cache-Control: no-cache\r\nAccept-Language: zh-cn,zh;q=0.5\r\n");
		if(!WinHttpAddRequestHeaders(net_request_, some_header.c_str(), some_header.length(), WINHTTP_ADDREQ_FLAG_ADD)){
			return false;
		}
		return true;
	}
    std::string WinDownload::Download(const size_t try_count,const char* file){
        IStream *stream = NULL;
		std::ofstream out;
        std::string message = "";
        for(size_t count=try_count;count!=0;count--)
        {
            if(!WinHttpSendRequest(net_request_, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
            {
				message = "failed";
                break;
            }
            if(!WinHttpReceiveResponse(net_request_,0))
            {
				message = "failed";
                break;
            }
            wchar_t szContentLength[32] = {0};
            DWORD cch = 64;
            DWORD dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
            WinHttpQueryHeaders(net_request_, WINHTTP_QUERY_CONTENT_LENGTH, NULL, &szContentLength, &cch, &dwHeaderIndex);
			if(file==NULL){
				if(CreateStreamOnHGlobal(0,TRUE,&stream))
				{
					message = "failed";
					break;
				}
			}
			else{
				out.open(file,std::ofstream::out|std::ofstream::binary);
				if(!out.is_open()){
					message = "failed";
					break;
				}
			}
            DWORD dwReceivedTotal = 0;
            while(WinHttpQueryDataAvailable(net_request_, &cch) && cch){
				char p[4096] = {0};
                if(cch > 4096)
                {
                    cch = 4096;
                }
                dwReceivedTotal += cch;
                WinHttpReadData(net_request_, p, cch, &cch);
				if(stream!=NULL){
					stream->Write(p,cch, NULL);
				}
				else{
					out.write(p,cch);
					out.flush();
				}
            }
			if(stream!=NULL){
				char* p = NULL;
				stream->Write(&p, 1, NULL);
				HGLOBAL hgl;
				if(GetHGlobalFromStream(stream, &hgl))
				{
					message = "failed";
					break;
				}
				p = reinterpret_cast<char *>(GlobalLock(hgl));
				if(!p)
				{
					message = "failed";
					break;
				}
				message.resize(0);
				message.append(p, dwReceivedTotal);
				GlobalUnlock(hgl);
			}
        }
        if(stream){
            stream->Release();
        }
		if(out.is_open()){
			out.close();
			out.clear();
		}
        return message;
    }
	void WinDownload::DownloadDone(){
		if(net_request_)
		{
			WinHttpCloseHandle(net_request_);
			net_request_ = NULL;
		}
		if(net_connect_)
		{
			WinHttpCloseHandle(net_connect_);
			net_connect_ = NULL;
		}
		if(net_open_)
		{
			WinHttpCloseHandle(net_open_);
			net_open_ = NULL;
		}
	}
}