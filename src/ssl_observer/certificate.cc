#include "ssl_observer/certificate.h"
#include "ssl_observer/main.h"

namespace ssl_observer{

	Certificate::Certificate() :cert_handle_(nullptr){
	}
	Certificate::Certificate(std::function<std::vector<std::uint8_t>(const char*)> read_file, const char* filename) : cert_handle_(nullptr){
		if (!Create(read_file,filename)){
			return;
		}
	}
	Certificate::Certificate(const unsigned char* src, const unsigned int length) : cert_handle_(nullptr){
		if (!Create(src, length)){
			return;
		}
	}
	Certificate::~Certificate(){
		Close();
	}
	bool Certificate::Create(std::function<std::vector<std::uint8_t>(const char*)> read_file, const char* filename){
		std::vector<std::uint8_t> result = read_file(filename);
		cert_handle_ = (CertHandle)CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (BYTE*)&result[0], result.size());
		return IsOpen();
	}
	bool Certificate::Create(const unsigned char* src, const unsigned int length){
		cert_handle_ = (CertHandle)CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (BYTE*)src, length);
		return IsOpen();
	}
	void Certificate::Close(){
		if (cert_handle_){
			CertFreeCertificateContext(reinterpret_cast<PCCERT_CONTEXT>(cert_handle_));
			cert_handle_ = nullptr;
		}
	}
	std::vector<std::wstring> Certificate::GetRevokeDistributionPoint(){
		std::vector<std::wstring> result;
		ULONG urlArraySize = 0;
		CryptGetObjectUrl(URL_OID_CERTIFICATE_CRL_DIST_POINT, reinterpret_cast<void*>(Handle()),
			CRYPT_GET_URL_FROM_EXTENSION,
			NULL,
			&urlArraySize,
			NULL,
			NULL,
			NULL);
		if (urlArraySize){
			PCRYPT_URL_ARRAY urlArray = reinterpret_cast<PCRYPT_URL_ARRAY>(HeapAlloc(GetProcessHeap(), 0, urlArraySize + 1));
			CryptGetObjectUrl(URL_OID_CERTIFICATE_CRL_DIST_POINT,
				reinterpret_cast<void*>(Handle()),
				CRYPT_GET_URL_FROM_EXTENSION,
				urlArray,
				&urlArraySize,
				NULL,
				NULL,
				NULL);
			const wchar_t** psrc = const_cast<const wchar_t**>(urlArray->rgwszUrl);
			for (uint32_t i = 0; i < urlArray->cUrl; i++){
				result.push_back(psrc[i]);
			}
			HeapFree(GetProcessHeap(), 0, urlArray);
		}
		return result;
	}
	bool Certificate::IsOpen() const{
		return (cert_handle_ != nullptr);
	}
	Certificate::CertHandle Certificate::Handle() const{
		return cert_handle_;
	}
}