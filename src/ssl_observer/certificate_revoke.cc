#include "ssl_observer/certificate_revoke.h"
#include <clx/uri.h>
#include <clx/html.h>
#include "thirdparty/googleurl/src/gurl.h"
#include "ssl_observer/main.h"
#if defined(OS_WIN)
#include <atlconv.h>
#include <atlexcept.h>
#include "ssl_observer/win_download.h"
#endif

namespace ssl_observer{
	CertificateRevoke* CertificateRevoke::GetInstance(){
		static CertificateRevoke* info;
		if (!info){
			CertificateRevoke* new_info = new CertificateRevoke;
			if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&info), new_info, NULL)){
				delete new_info;
			}
		}
		return info;
	}
	CertificateRevoke::CertificateRevoke(){
		handle_map_.clear();
	}
	CertificateRevoke::~CertificateRevoke(){
		Clear();
	}
	void CertificateRevoke::Add(std::function<std::wstring(void)> directory,
		std::function<std::vector<std::uint8_t>(const char*)> read_file,
		std::vector<std::wstring> url_list){
		std::vector<std::wstring>::iterator it;
		for (it = url_list.begin(); it != url_list.end(); it++){
			std::wstringstream stream;
			stream << std::hex << std::setfill(L'0') << directory() << std::hash<std::wstring>()(it->c_str()) << ".crl";
			std::wstring file(stream.str());
			if(!PathFileExistsW(file.c_str())){
				Download(it->c_str(), file.c_str());
			}
			if(handle_map_.find(it->c_str())==handle_map_.end()){
				USES_CONVERSION;
				LOG(plog::debug) << "revoke url:" << it->c_str();
				LOG(plog::debug) << "local file:" << file.c_str();
				std::vector<std::uint8_t> revoke_list = read_file(W2A(file.c_str()));
				handle_map_[it->c_str()] = (RevokeHandle)CertCreateCRLContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (BYTE*)&revoke_list[0], revoke_list.size());
			}
		}
	}
	bool CertificateRevoke::Download(const std::wstring& url, const std::wstring& file){
		GURL gurl(url);
		if (!gurl.is_valid()){
			return true;
		}
#if defined(OS_WIN)
		WinDownload download;
		USES_CONVERSION;
		if (!download.CreateConnect(A2W(gurl.host().c_str()), gurl.port().c_str())){
			return true;
		}
		if (!download.AddDownloadHeader(ATL::CA2W(clx::uri::decode(gurl.path()).c_str(), CP_UTF8), L"")){
			return true;
		}
		if (download.Download(1, W2A(file.c_str())) != "failed"){
			return true;
		}
		//URLDownloadToFileW(0, url.c_str(), file.c_str(), 0, NULL);
#endif
		return false;
	}
	bool CertificateRevoke::IsRevoke(const void* cert){
		std::map<std::wstring, RevokeHandle>::iterator it;
		for (it = handle_map_.begin(); it != handle_map_.end(); it++){
			PCRL_ENTRY crl_entry = nullptr;
			BOOL result = CertFindCertificateInCRL(reinterpret_cast<PCCERT_CONTEXT>(cert),reinterpret_cast<PCCRL_CONTEXT>(it->second), 0, 0, &crl_entry);
			if (result&&crl_entry != nullptr){
				return true;
			}
		}
		return false;
	}
	void CertificateRevoke::Clear(){
		std::map<std::wstring, RevokeHandle>::iterator it;
		for (it = handle_map_.begin(); it != handle_map_.end(); it++){
			CertFreeCRLContext(reinterpret_cast<PCCRL_CONTEXT>(it->second));
		}
		handle_map_.clear();
	}
}