#include "ssl_observer/main.h"
#include <clx/http.h>
#include <clx/uri.h>
#include <thirdparty/googleurl/src/gurl.h>
#include "ssl_observer/certificate.h"
#include "ssl_observer/certificate_map.h"

namespace ocsp{
	std::vector<std::uint8_t> SerialNumber(const char* file){
		std::vector<std::uint8_t> result;
		X509* cert = ssl_observer::Load((char*)file, 0, nullptr);
		ASN1_INTEGER* serial = X509_get_serialNumber(cert);
		result.resize(serial->length);
		memmove(result.data(), serial->data, serial->length);
		X509_free(cert);
		return result;
	}
	std::vector<std::uint8_t> OCSPFormat(const std::vector<std::uint8_t>& serial){
		std::uint8_t based[] = {
			0x30, 0x4e, 0x30, 0x4c, 0xa0, 0x03, 0x02, 0x01
			, 0x00, 0x30, 0x45, 0x30, 0x43, 0x30, 0x41, 0x30
			, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a
			, 0x05, 0x00, 0x04, 0x14, 0x00, 0xeb, 0x0c, 0xcc
			, 0xf4, 0x71, 0x6c, 0xba, 0x46, 0x23, 0xa3, 0xd5
			, 0x2f, 0x98, 0xd4, 0x3d, 0xf6, 0x30, 0xa9, 0x1d
			, 0x04, 0x14, 0x88, 0x27, 0x17, 0x09, 0xa9, 0xb6
			, 0x18, 0x60, 0x8b, 0xec, 0xeb, 0xba, 0xf6, 0x47
			, 0x59, 0xc5, 0x52, 0x54, 0xa3, 0xb7, 0x02, 0x08 };
		std::vector<std::uint8_t> result;
		result.resize(sizeof(based)+serial.size());
		memmove(result.data(), based, sizeof(based));
		memmove(result.data() + sizeof(based), serial.data(), serial.size());
		return result;
	}
	std::vector<std::uint8_t> OCSPEncode(const std::vector<std::uint8_t>& ocsp_str){
		std::vector<std::uint8_t> based_64;
		std::vector<std::uint8_t> result;
		based_64.resize(4096);
		int len = EVP_EncodeBlock(based_64.data(), ocsp_str.data(), ocsp_str.size());
		result.resize((len!=0&&len!=-1) ? len+1 : 0);
		memmove(result.data(), based_64.data(), result.size());
		return result;
	}
	std::vector<GURL> OCSPURL(const std::vector<std::uint8_t>& ocsp_enc){
		std::vector<GURL> host;
		host.push_back(GURL(std::string("http://ocsp.apple.com/ocsp-wwdr01/") + std::string(reinterpret_cast<const char*>(ocsp_enc.data()))));
		host.push_back(GURL(std::string("http://ocsp.apple.com/ocsp02-wwdr01/") + std::string(reinterpret_cast<const char*>(ocsp_enc.data()))));
		return host;
	}
}

int main(int _argc,char** _argv,char** _env){
	enum class CERTStatus{
		kGood = 0,
		kRevoke = 1,
		kUnknown = 2,
		kNotPased = 0xff
	};
	std::vector<std::uint8_t> serial_number = ocsp::SerialNumber(_argv[1]);
	std::vector<GURL> urls = ocsp::OCSPURL(ocsp::OCSPEncode(ocsp::OCSPFormat(serial_number)));
	std::vector<GURL>::iterator iter;
	for (iter = urls.begin(); iter!=urls.end(); iter++){
		try {
			const std::string port = iter->port();
			clx::http session(clx::uri::encode(iter->host()), port.empty() ? 80 : atoi(port.c_str()));
			session.get(clx::uri::encode(iter->path()));
			const std::string response = session.body();
			if (session.code() == 200 && response.size()>1024){
				std::vector<std::uint8_t> ocsp_responser;
				ocsp_responser.resize(response.size());
				memmove(ocsp_responser.data(), response.c_str(), response.length());
				std::vector<std::uint8_t>::iterator it;
				std::vector<std::uint8_t>::iterator cert_status;
				for (it = serial_number.begin(); it != serial_number.end(); it++){
					cert_status = std::find(ocsp_responser.begin(), ocsp_responser.end(), *it);
				}
				cert_status++;
				switch (*cert_status & 0x1f)
				{
				case CERTStatus::kGood:
					printf("Good\r\n");
					break;
				case CERTStatus::kRevoke:
					printf("Revoke\r\n");
					break;
				case CERTStatus::kUnknown:
					printf("Unknown\r\n");
					break;
				case CERTStatus::kNotPased:
					printf("NotPased\r\n");
					break;
				default:
					break;
				}
				break;
			}
		}
		catch (clx::socket_error& e) {
			std::cerr << e.what() << std::endl;
			continue;
		}
	}
	return 0;
}