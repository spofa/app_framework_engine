#ifndef SSL_OBSERVER_CERTIFICATE_MAP_H_
#define SSL_OBSERVER_CERTIFICATE_MAP_H_

#include <cstdint>
#include <map>
#include <vector>
#include <string>

namespace ssl_observer{
	class CertificateMap
	{
	public:
		using ExtMap = std::map < std::string, std::string>;
		CertificateMap();
		explicit CertificateMap(const char* file);
		~CertificateMap();
		ExtMap GetCertificateExtensions(const char* file = nullptr);
	private:
		CertificateMap(const CertificateMap&) = delete;
		CertificateMap& operator=(const CertificateMap&) = delete;
		ExtMap extensions_;
		std::string files_;

	};
}

#endif