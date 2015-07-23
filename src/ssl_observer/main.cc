#include "ssl_observer\main.h"
#include <clx/uri.h>
#include "ssl_observer\certificate.h"
#include "ssl_observer\certificate_revoke.h"
#include "ssl_observer\function_type.h"
#include "ssl_observer\certificate_map.h"
#include "ssl_observer\certificate_ocsp.h"
#if defined(OS_WIN)
#include <atlconv.h>
#include <atlexcept.h>
#endif

namespace ssl_observer{
	namespace LibEventServer{
		struct evhttp* http_services = nullptr;
		void SendResponse(struct evhttp_request* request, const std::string& msg, int code = HTTP_OK){
			if (request != NULL){
				evhttp_add_header(request->output_headers, "Server", "AppChinaCertificateServer/1.0");
				evhttp_add_header(request->output_headers, "Content-Type", "text/plain;charset=UTF-8");
				evhttp_add_header(request->output_headers, "Connection", "close");
				struct evbuffer *buf = evbuffer_new();
				evbuffer_add_printf(buf, "%s", msg.c_str());
				evhttp_send_reply(request, code, "OK", buf);
				evbuffer_free(buf);
			}
		}
		int ParseURI(const char *dst, const char *src){
			int ch1, ch2;
			for (int len = strlen(src);; len--){
				if (((ch1 = (unsigned char)(*(dst++))) >= 'A') && (ch1 <= 'Z')){
					ch1 += 0x20;
				}
				if (((ch2 = (unsigned char)(*(src++))) >= 'A') && (ch2 <= 'Z')){
					ch2 += 0x20;
				}
				if (!ch2 || (ch1 != ch2) || len == 0){
					ch1 = (ch1 == '?' || ch1 == 0) ? 0 : 1;
					break;
				}
			}
			return(ch1 - ch2);
		}
		const char* ParsePyloadData(struct evhttp_request *req){
			if (req != NULL){
				char* pyload = (char*)EVBUFFER_DATA(req->input_buffer);
				if(pyload){
					pyload[EVBUFFER_LENGTH(req->input_buffer)] = 0;
				}
				return pyload;
			}
			return "";
		}
		void MessageLoop(struct evhttp_request *req, void *arg){
			std::string relative_uri = evhttp_request_uri(req);
			if (EVHTTP_REQ_GET == req->type&&!ParseURI(relative_uri.c_str(), "/CheckUrl")){
				struct evkeyvalq params = { 0 };
				evhttp_parse_query_str(&relative_uri[relative_uri.find('?')+sizeof(char)], &params);
				const char* url = evhttp_find_header((struct evkeyvalq*)&params, "url");
				if(!url){
					std::ostringstream response_message;
					response_message << "ERROR:" << "#File:" << __FILE__ << "#Function:" << __FUNCTION__ << "#Line:" << __LINE__ << std::endl;
					SendResponse(req, response_message.str());
					return;
				}
				std::wstringstream stream;
				stream << std::hex << std::setfill(L'0') << ssl_observer::CurDirectory() << std::hash<std::string>()(url) << ".cer";
				std::wstring file(stream.str());
				USES_CONVERSION;
				const wchar_t* w_url = ATL::CA2W(clx::uri::decode(url).c_str(), CP_UTF8);
				ssl_observer::CertificateRevoke::GetInstance()->Download(w_url, file);
				if (!PathFileExistsW(file.c_str())){
					std::ostringstream response_message;
					response_message << "ERROR:" << "#File:" << __FILE__ << "#Function:" << __FUNCTION__ << "#Line:" << __LINE__ << std::endl;
					SendResponse(req, response_message.str());
					return;
				}
				ssl_observer::Certificate certificate(ssl_observer::Read,W2A(file.c_str()));
				if(!certificate.IsOpen()){
					std::ostringstream response_message;
					response_message << "ERROR:" << "#File:" << __FILE__ << "#Function:" << __FUNCTION__ << "#Line:" << __LINE__ << std::endl;
					SendResponse(req, response_message.str());
					return;
				}
				ssl_observer::CertificateRevoke::GetInstance()->Add(ssl_observer::CurDirectory, ssl_observer::Read, certificate.GetRevokeDistributionPoint());
				if (ssl_observer::CertificateRevoke::GetInstance()->IsRevoke(certificate.Handle())){
					std::ostringstream response_message;
					response_message << "Revoke";
					SendResponse(req, response_message.str());
				}
				else{
					std::ostringstream response_message;
					response_message << "Available";
					SendResponse(req, response_message.str());
				}
				DeleteFileW(file.c_str());
			}
			else{
				std::ostringstream response_message;
				response_message << "ERROR:" << "#File:" << __FILE__ << "#Function:" << __FUNCTION__ << "#Line:" << __LINE__ << std::endl;
				SendResponse(req, response_message.str());
				return;
			}
		}
		bool Initialize(const int port){
			WSADATA WSAData;
			WSAStartup(0x101, &WSAData);
			event_init();
			http_services = evhttp_start("0.0.0.0", port);
			return (http_services != NULL);
		}
		void Uninitialize(){
			evhttp_free(http_services);
			http_services = NULL;
		}
		void Run(const int time_out){
			evhttp_set_timeout(http_services, time_out);
			evhttp_set_gencb(http_services, MessageLoop, NULL);
			event_dispatch();
		}
	}
}

int main(int argc,char* argv[]){

	std::uint8_t s_1 = 0x80 & 0x1f;
	std::uint8_t s_2 = 0xA1 & 0x1f;
	ssl_observer::OCSPVerify("C:\\Users\\xxx\\Desktop\\mobileprovision\\cer\\ecmc.cer");
	std::vector<std::wstring> url_list;
	plog::init(plog::debug, "certificate.txt");
	url_list.push_back(L"http://developer.apple.com/certificationauthority/wwdrca.crl");
	ssl_observer::CertificateRevoke::GetInstance()->Add(ssl_observer::CurDirectory, ssl_observer::Read, url_list);
	ssl_observer::LibEventServer::Initialize(8080);
	ssl_observer::LibEventServer::Run(120);
	ssl_observer::LibEventServer::Uninitialize();
	return 1;
}
