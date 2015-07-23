#ifndef SSL_OBSERVER_CERTIFICATE_REVOKE_H_
#define SSL_OBSERVER_CERTIFICATE_REVOKE_H_

#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <functional>

namespace ssl_observer{
	class CertificateRevoke
	{
	public: 
		using RevokeHandle = void*;
		CertificateRevoke();
		~CertificateRevoke();
		void Add(std::function<std::wstring(void)> directory,std::function<std::vector<std::uint8_t>(const char*)> read_file, std::vector<std::wstring> url_list);
		bool Download(const std::wstring& url, const std::wstring& file);
		bool IsRevoke(const void* cert);
		void Clear();
		static CertificateRevoke* GetInstance();
	private:
		CertificateRevoke(const CertificateRevoke&) = delete;
		CertificateRevoke& operator=(const CertificateRevoke&) = delete;
		std::map<std::wstring,RevokeHandle> handle_map_;
	};
}


#endif // !SSL_OBSERVER_CERTIFICATE_REVOKE_H_
