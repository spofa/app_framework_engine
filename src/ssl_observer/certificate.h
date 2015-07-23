#ifndef SSL_OBSERVER_CERTIFICATE_H_
#define SSL_OBSERVER_CERTIFICATE_H_

#include <cstdint>
#include <vector>
#include <string>
#include <functional>
#include "ssl_observer/main.h"

namespace ssl_observer{
	static X509* Load(BIO *cert, char * pwd){
		X509 * x = NULL;
		if (!x){
			BIO_reset(cert);
			x = d2i_X509_bio(cert, NULL);
		}
		if (!x){
			BIO_reset(cert);
			x = PEM_read_bio_X509(cert, NULL, NULL, NULL);
		}
		if (!x){
			BIO_reset(cert);
			PKCS12 *p12 = d2i_PKCS12_bio(cert, NULL);
			PKCS12_parse(p12, pwd, NULL, &x, NULL);
			PKCS12_free(p12);
			p12 = NULL;
		}
		return x;
	}
	static X509* Load(char* cert, int certlen, char * pass){
		BIO * in = NULL;
		if ((in = BIO_new_file(cert, "rb")) == NULL){
			if ((in = BIO_new_mem_buf(cert, certlen)) == NULL){
				return NULL;
			}
		}
		X509* x509 = Load(in, pass);
		if (in != NULL){
			BIO_free(in);
		}
		return x509;
	}
	static EVP_PKEY* LoadKey(BIO* bio, char *pass){
		EVP_PKEY *pkey = NULL;
		if (!pkey){
			BIO_reset(bio);
			pkey = d2i_PrivateKey_bio(bio, NULL);
		}
		if (!pkey){
			BIO_reset(bio);
			pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, pass);
		}
		if (!pkey){
			BIO_reset(bio);
			PKCS12 *p12 = d2i_PKCS12_bio(bio, NULL);
			PKCS12_parse(p12, pass, &pkey, NULL, NULL);
			PKCS12_free(p12);
			p12 = NULL;
		}
		return pkey;
	}
	static EVP_PKEY* LoadKey(char * key, int keylen, char * pass){
		BIO * in = NULL;
		if ((in = BIO_new_file(key, "rb")) == NULL){
			if ((in = BIO_new_mem_buf(key, keylen)) == NULL){
				return NULL;
			}
		}
		EVP_PKEY* pkey = LoadKey(in, pass);
		if (in != NULL){
			BIO_free(in);
		}
		return pkey;
	}
	class Certificate{
	public:
		using CertHandle = void*;
		Certificate();
		Certificate(std::function<std::vector<std::uint8_t>(const char*)> read_file, const char* filename);
		Certificate(const unsigned char* src, const unsigned int length);
		~Certificate();
		bool Create(std::function<std::vector<std::uint8_t>(const char*)> read_file, const char* filename);
		bool Create(const unsigned char* src, const unsigned int length);
		void Close();
		std::vector<std::wstring> GetRevokeDistributionPoint();
		bool IsOpen() const;
		CertHandle Handle() const;
	private:
		Certificate(const Certificate&) = delete;
		Certificate& operator=(const Certificate&) = delete;
		CertHandle cert_handle_;
	};

}

#endif // !SSL_OBSERVER_CERTIFICATE_H_